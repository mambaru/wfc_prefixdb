#include "rocksdb.hpp"
#include "persistent_value.hpp"
//#include "updater/operation_inc.hpp"
//#include "updater/operation_set.hpp"
#include "updater/update.hpp"

#include <wfc/logger.hpp>
#include <wfc/json.hpp>
#include <rocksdb/db.h>
#include <rocksdb/write_batch.h>

namespace wamba{ namespace prefixdb {

rocksdb::rocksdb( db_type* db, std::shared_ptr<iprefixdb> repli)
  : _db(db)
  , _repli(repli)
{}

// Гарантированно req!=nullptr, may be cb==nullptr
void rocksdb::set( request::set::ptr req, response::set::handler cb)
{
  ::rocksdb::WriteBatch batch;

  for ( const auto& field : req->fields)
  {
    batch.Put( field.first, field.second );
  }
  
  this->write_batch_<response::set>(batch, std::move(req), std::move(cb) );
  
  /*
  size_t reserve = 0;
  for ( const request::set::field& field : req->fields)
  {
    reserve += field.val.size() + sizeof(value_head);
  }
  
  persistent_value::buffer_type buff;
  buff.reserve(reserve);
  
  for ( const request::set::field& field : req->fields)
  {
    if ( field.force )
    {
      auto slice = persistent_value::serialize<slice_type>(buff, field.val, field.type, field.ttl);
      batch.Put(field.key, slice );
    }
    else
    {
      // Требуеться проверка типа, работаем через Merge
      
      operation_set op;
      op.force = field.force; // всегда false
      op.val   = field.val;
      op.type  = field.type;
      op.ttl   = field.ttl;
      auto slice = operation_set::serialize<slice_type>(buff, op );
      
      batch.Merge(field.key, slice);
    }
  }

  this->write_batch_<response::set>(batch, std::move(req), std::move(cb) );
  */


  /*
  ::rocksdb::Status status = _db->Write( ::rocksdb::WriteOptions(), &batch);

  if ( cb == nullptr )
    return;

  if ( req->nores )
  {
    auto res = std::make_unique<response::inc>();
    res->prefix = std::move(req->prefix);
    res->status = status.ok() ? common_status::OK : common_status::WriteError;
    cb( std::move(res) );
  }
  else
  {
    std::cout << "GET " << req->fields[0].key << std::endl;
    this->get_<response::inc>( std::move(req), std::move(cb) );
  }
  */

  /*
  // **********************************************************
  // формируем ответ
  auto res = std::make_unique<response::set>();
  res->prefix = std::move(req->prefix);

  if ( status.ok() )
  {
    // Если нужен статус по каждому полю
    if ( !req->nores )
    {
      res->fields.reserve( req->fields.size() );
      for ( const auto& field : req->fields)
      {
        field_type rf;
        rf.key = std::move( field.key );
        // если нужно записанное значение в ответе
        if ( !req->noval )
        {
          rf.val = std::move( field.val );
        }
        res->fields.push_back( std::move(rf) );
      }
    }
    res->status = common_status::OK;
  }
  else if (res!=nullptr)
  {
    res->status = common_status::WriteError;
    COMMON_LOG_ERROR("rocksdb::set WriteError: " << status.ToString() )
  }

  cb(std::move(res));
  */
}

template<typename Res, typename Batch, typename ReqPtr, typename Callback>
void rocksdb::write_batch_(Batch& batch, ReqPtr req, Callback cb)
{
  ::rocksdb::WriteOptions wo;
  wo.sync = req->sync;
  
  ::rocksdb::Status status = _db->Write( ::rocksdb::WriteOptions(), &batch);

  if ( cb == nullptr )
    return;

  if ( req->nores )
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


namespace {
  /*
  inline void get_mov_val(response::has::field&,     std::string&    ) {  }
  inline void get_mov_val(response::set::field& fld, std::string& val) {    fld.val = std::move(val);  }
  inline void get_mov_val(response::get::field& fld, std::string& val) {    fld.val = std::move(val);  }
  inline void get_mov_val(response::del::field& fld, std::string& val) {    fld.val = std::move(val);  }
  inline void get_mov_val(response::inc::field& fld, std::string& val) {    fld.val = std::move(val);  }
  */
  
inline std::string& get_key(std::string& key) {return key;}
inline std::string& get_key( std::pair<std::string, std::string>& field) {return field.first;}
}

template<typename Res, typename ReqPtr, typename Callback>
void rocksdb::get_(ReqPtr req, Callback cb)
{
  typedef Res response_type;
  typedef ::rocksdb::Slice slice_type;

  std::vector<slice_type> keys;
  keys.reserve(req->fields.size() );
  for ( auto& fld: req->fields ) keys.push_back( get_key(fld) );

  std::vector<std::string> resvals;
  resvals.reserve(keys.size());
  std::vector< ::rocksdb::Status> status
    = _db->MultiGet( ::rocksdb::ReadOptions(), keys, &resvals);

  bool ok = true;
  auto res = std::make_unique<response_type>();
  res->prefix = std::move(req->prefix);
  res->status = common_status::OK;
  res->fields.reserve(resvals.size());
  field_pair field;
  for ( size_t i = 0; i!=resvals.size(); ++i)
  {
    field.first = std::move( get_key(req->fields[i]) );
    field.second = std::move(resvals[i]);
    res->fields.push_back( std::move(field) );
    if ( !status[i].ok() )
    {
      ok = false;
      res->fields.back().second = "null";
    }
    /*
    field_pair field(std::move(keys[i]), std::move(resvals[i]));
    if ( !status[i].ok() )
    {
      ok = false;
      field.second = "null";
    }
    res->fields.push_back( std::move(field) );
    */
  }
  
  if (!ok) res->status = common_status::SomeFieldFail;
  cb( std::move(res) );


    /*
  if ( keys.size() != resvals.size() )
  {
    DOMAIN_LOG_FATAL("rocksdb::get keys.size() != resvals.size() " << keys.size() << "!=" << resvals.size())
    abort();
  }

  // **********************************************************
  // формируем ответ

  bool ok = true;
  auto res = std::make_unique<response_type>();
  res->prefix = std::move(req->prefix);
  res->status = common_status::OK;
  res->fields.reserve(resvals.size());
  for ( size_t i = 0; i!=resvals.size(); ++i)
  {
    typename response_type::field fld;
    fld.key = std::move( req->fields[i].key );
    if ( status[i].ok() )
    {
      const std::string& str = resvals[i];
      persistent_value val = persistent_value::deserialize( str.data(), str.data() + str.size(), req->noval );
      get_mov_val(fld, val.data);
      fld.type = val.type;
      fld.ttl = val.ttl;
    }
    else
    {
      fld.type = field_type::none;
      fld.ttl = 0;
      std::string tmp = "null";
      get_mov_val(fld, tmp);
    }
    res->fields.push_back( std::move(fld) );
    if ( ok ) ok = status[i].ok();
  }

  if (!ok) res->status = common_status::SomeFieldFail;
  cb( std::move(res) );
  */
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
  for ( auto& field : req->fields)
  {
    batch.Delete( get_key(field) );
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
#warning сначала удалить, потом взять 
    //bool noval = req->noval;
    this->get_<response::del>( std::move(req), [this, &batch, &cb/*, noval*/](response::del::ptr res)
    {
      ::rocksdb::Status status = this->_db->Write( ::rocksdb::WriteOptions(), &batch);
      res->status = status.ok() ? common_status::OK : common_status::WriteError;
      
      // Очищаем, если значения не нужны. Один хрен они были запрошены и перемещены. 
      // Имеет смысл только на больших строчках.
      // TODO: не надо 
      //if ( noval ) for (auto& fld : res->fields) fld.val.clear();

      cb( std::move(res) );
    });
  }
}

void rocksdb::inc( request::inc::ptr req, response::inc::handler cb) 
{
  ::rocksdb::WriteOptions wopt;
  wopt.sync = req->sync;
  ::rocksdb::WriteBatch batch;
  std::string json;
  json.reserve(64);
  update_json::serializer ser;
  for (auto& f: req->fields)
  {
    update upd;
    upd.mode = update_mode::inc;
    upd.value = std::move(f.second);
    json.clear();
    ser( upd, std::inserter(json, json.end()));
    batch.Merge(f.first, json);
  }
  
  this->write_batch_<response::inc>(batch, std::move(req), std::move(cb) );
  /*
  update upd;
  upd.mode = update_mode::si;
  upd.force = req->force;
  upd.value = req->
  */
  
  
  
  /*
  typedef ::rocksdb::Slice slice_type;
  ::rocksdb::WriteBatch batch;

  size_t reserve = req->fields.size() * sizeof(operation_inc);
  operation_inc::buffer_type buff;
  buff.reserve(reserve);
  
  for ( const auto& field : req->fields)
  {
    operation_inc op;
    op.force = field.force;
    op.inc   = field.inc;
    op.def   = field.def;
    op.ttl   = field.ttl;

    
    auto slice = operation_inc::serialize<slice_type>(buff, op );
    batch.Merge(field.key, slice);
  }

  ::rocksdb::Status status = _db->Write( ::rocksdb::WriteOptions(), &batch );
  if ( cb == nullptr )
    return;
  
  if ( req->nores )
  {
    auto res = std::make_unique<response::inc>();
    res->prefix = std::move(req->prefix);
    res->status = status.ok() ? common_status::OK : common_status::WriteError;
    cb( std::move(res) );
  }
  else
  {
    std::cout << "GET " << req->fields[0].key << std::endl;
    this->get_<response::inc>( std::move(req), std::move(cb) );
  }
  */
}


void rocksdb::packed( request::packed::ptr req, response::packed::handler cb)
{
  ::rocksdb::WriteOptions wopt;
  wopt.sync = req->sync;
  
  ::rocksdb::WriteBatch batch;
  std::string json;
  json.reserve(64);
  
  update_json::serializer ser;
  for (auto& f: req->fields)
  {
    update upd;
    upd.mode = update_mode::packed;
    upd.value = std::move(f.second);
    json.clear();
    ser( upd, std::inserter(json, json.end()));
    
    std::cout << "rocksdb::packed Merge.... : " << json << std::endl;
    batch.Merge(f.first, json);
  }

  this->write_batch_<response::packed>(batch, std::move(req), std::move(cb) );
}

void rocksdb::upd( request::upd::ptr req, response::upd::handler cb) 
{
  /*
  typedef std::vector< std::pair<std::string, std::string> > object_type;
  std::string jsob = "{\"key1\":\"value1\", \"key2\":\"value2\"}";
  std::cout << jsob << std::endl;
  typedef ::wfc::json::object2array<  
    ::wfc::json::value<std::string>,
    ::wfc::json::raw_value<std::string>
  > object_json;
  
  object_type obj;
  object_json::serializer()(obj, jsob.begin(), jsob.end() );
  
  for (auto i : obj )
  {
    std::cout << i.first << ":" << i.second << std::endl;
  }
  std::string res;
  object_json::serializer()(obj, std::back_inserter(res) );
  std::cout << res << std::endl;
  */
  
  /*DOMAIN_LOG_FATAL("rocksdb::upd not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  
  abort();
  */
}

}}
