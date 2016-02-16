#include "rocksdb.hpp"
//#include "persistent_value.hpp"
#include "merge/merge.hpp"
#include "merge/merge_json.hpp"

#include <wfc/logger.hpp>
#include <wfc/json.hpp>
#include <rocksdb/db.h>
#include <rocksdb/write_batch.h>
#include <rocksdb/iterator.h>

namespace wamba{ namespace prefixdb {

namespace {
  inline std::string& get_key(std::string& key) {return key;}
  inline std::string& get_key( std::pair<std::string, std::string>& field) {return field.first;}
}

rocksdb::rocksdb( db_type* db, std::shared_ptr<iprefixdb> repli)
  : _db(db)
  , _repli(repli)
{}

template<merge_mode Mode, typename Res, typename ReqPtr, typename Callback>
void rocksdb::merge_(ReqPtr req, Callback cb)
{
  ::rocksdb::WriteBatch batch;
  std::string json;
  json.reserve(64);
  merge_json::serializer ser;
  for (auto& f: req->fields)
  {
    merge upd;
    upd.mode = Mode;
    upd.raw = std::move(f.second);
    json.clear();
    ser( upd, std::inserter(json, json.end()));
    std::cout << "rocksdb::merge_ " << json << std::endl;
    batch.Merge(f.first, json);
  }
  
  this->write_batch_<Res>(batch, std::move(req), std::move(cb) );
}

template<typename Res, typename Batch, typename ReqPtr, typename Callback>
void rocksdb::write_batch_(Batch& batch, ReqPtr req, Callback cb)
{
  ::rocksdb::WriteOptions wo;
  wo.sync = req->sync;
  
  ::rocksdb::Status status = _db->Write( ::rocksdb::WriteOptions(), &batch);

  if ( cb == nullptr )
    return;

  if ( req->nores || !status.ok() )
  {
    auto res = std::make_unique<Res>();
    res->prefix = std::move(req->prefix);
    res->status = status.ok() ? common_status::OK : common_status::WriteError;
    cb( std::move(res) );
  }
  else
  {
    this->get_<Res>( std::move(req), std::move(cb) );
  }
}

template<typename Res, typename ReqPtr, typename Callback>
void rocksdb::get_(ReqPtr req, Callback cb)
{
  typedef Res response_type;
  typedef ::rocksdb::Slice slice_type;
  
  // Формируем список ключей для запроса
  std::vector<slice_type> keys;
  keys.reserve(req->fields.size() );
  for ( auto& fld: req->fields ) 
    keys.push_back( get_key(fld) );

  // Забираем значения
  std::vector<std::string> resvals;
  resvals.reserve(keys.size());
  std::vector< ::rocksdb::Status> status
    = _db->MultiGet( ::rocksdb::ReadOptions(), keys, &resvals);

  auto res = std::make_unique<response_type>();
  res->prefix = std::move(req->prefix);
  res->status = common_status::OK;
  res->fields.reserve(resvals.size());
  
  // Формируем результата
  field_pair field;
  for ( size_t i = 0; i!=resvals.size(); ++i)
  {
    field.first = std::move( get_key(req->fields[i]) );
    
    if ( req->noval )
    { // Если значения не нужны
      field.second = status[i].ok() ? "true" : "false";
    }
    else if ( status[i].ok() )
    { // Если ключ существет
      field.second = std::move(resvals[i]);
    }
    else
    { // Если ключа нет записывам null
      field.second = "null";
    }
    res->fields.push_back( std::move(field) );
  }
  
  cb( std::move(res) );
}

void rocksdb::set( request::set::ptr req, response::set::handler cb)
{
  ::rocksdb::WriteBatch batch;

  for ( const auto& field : req->fields)
  {
    batch.Put( field.first, field.second );
  }
  
  this->write_batch_<response::set>(batch, std::move(req), std::move(cb) );
}

void rocksdb::get( request::get::ptr req, response::get::handler cb)
{
  this->get_<response::get>( std::move(req), std::move(cb) );
}

void rocksdb::has( request::has::ptr req, response::has::handler cb)
{
  this->get_<response::has>( std::move(req), std::move(cb) );
}

void rocksdb::del( request::del::ptr req, response::del::handler cb) 
{
  ::rocksdb::WriteBatch batch;
  for ( auto& key : req->fields)
  {
    batch.Delete( key );
  }

  if ( req->nores || cb==nullptr)
  {
    ::rocksdb::Status status = _db->Write( ::rocksdb::WriteOptions(), &batch);
    if ( cb!=nullptr )
    {
      auto res = std::make_unique<response::del>();
      res->prefix = std::move(req->prefix);
      res->status = status.ok() ? common_status::OK : common_status::WriteError;
      cb( std::move(res) );
    }
  }
  else
  {
    this->get_<response::del>( std::move(req), [this, &batch, &cb](response::del::ptr res)
    {
      ::rocksdb::Status status = this->_db->Write( ::rocksdb::WriteOptions(), &batch);
      res->status = status.ok() ? common_status::OK : common_status::WriteError;
      cb( std::move(res) );
    });
  }
}


void rocksdb::inc( request::inc::ptr req, response::inc::handler cb) 
{
  this->merge_<merge_mode::inc, response::inc>( std::move(req), std::move(cb) );
}

void rocksdb::add( request::add::ptr req, response::add::handler cb) 
{
  this->merge_<merge_mode::add, response::add>( std::move(req), std::move(cb) );
}

void rocksdb::packed( request::packed::ptr req, response::packed::handler cb)
{  
  this->merge_<merge_mode::packed, response::packed>( std::move(req), std::move(cb) );
}

void rocksdb::range( request::range::ptr req, response::range::handler cb)
{
  DEBUG_LOG_MESSAGE("range from '" << req->from << " to '" << req->to )
  typedef ::rocksdb::Iterator iterator_type;
  typedef ::rocksdb::Slice slice_type;
  
  typedef std::shared_ptr<iterator_type> iterator_ptr;
  auto res=std::make_unique<response::range>();
  res->status = common_status::OK;
  res->prefix = std::move(req->prefix);
  res->fin = false;
  
  ::rocksdb::ReadOptions opt;
  iterator_ptr itr(_db->NewIterator(opt));
  if ( itr != nullptr )
  {
    field_pair field;
    
    // offset
    itr->Seek( req->from );
    while ( req->offset && itr->Valid() ) 
    {
      req->offset--; itr->Next(); 
    }
    
    while ( req->limit && itr->Valid() )
    {
      slice_type key = itr->key();
      
      field.first.assign(key.data(), key.data() + key.size() );
      DEBUG_LOG_MESSAGE("key " << field.first << " > " << req->to << "? " << (field.first > req->to) )
      
      if ( !req->to.empty() && field.first > req->to )
        break;
      
      if ( !req->noval )
      {
        auto val = itr->value();
        field.second.assign(val.data(), val.data() + val.size() );
      }
      res->fields.push_back(std::move(field));
      itr->Next();
      req->limit--;
    }
    
    res->fin |= req->limit!=0;
    res->fin |= !itr->Valid();
    if ( !res->fin )
    {
      itr->Next();
      res->fin = !itr->Valid() || itr->value()==req->to;
    }
  }
  cb( std::move(res) );
}

}}
