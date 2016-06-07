#include "wrocksdb.hpp"
#include "wrocksdb_slave.hpp"
#include "merge/merge.hpp"
#include "merge/merge_json.hpp"
#include "../aux/base64.hpp"

#include <prefixdb/logger.hpp>
#include <wfc/json.hpp>
#include <wfc/core/global.hpp>
#include <rocksdb/db.h>
#include <rocksdb/write_batch.h>
#include <rocksdb/iterator.h>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <chrono>
#include <string>


#include "../aux/copy_dir.hpp"


namespace wamba{ namespace prefixdb {

namespace 
{
  inline std::string& get_key(std::string& key) {return key;}
  inline std::string& get_key( std::pair<std::string, std::string>& field) {return field.first;}
}

wrocksdb::wrocksdb( std::string name, const db_config conf,  db_type* db)
  : _name(name)
  , _conf(conf)
  , _db1(db)
{
  if ( conf.slave.enabled )
    _slave = std::make_shared<wrocksdb_slave>(name, conf.path, conf.slave, *db);
  _flow = conf.args.workflow;
}


void wrocksdb::start( ) 
{
  std::lock_guard<std::mutex> lk(_mutex);
  if ( _slave )  _slave->start();
}



void wrocksdb::stop_()
{
  PREFIXDB_LOG_MESSAGE("preffix DB stop for prefix=" << _name)
  if ( _slave ) _slave->stop();
  _slave = nullptr;
  _db1=nullptr;
}

void wrocksdb::stop()
{
  std::lock_guard<std::mutex> lk(_mutex);
  this->stop_();
}

template<merge_mode Mode, typename Res, typename ReqPtr, typename Callback>
void wrocksdb::merge_(ReqPtr req, Callback cb)
{
  auto batch = std::make_shared< ::rocksdb::WriteBatch >();
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
    batch->Merge(f.first, json);
  }
  
  this->write_batch_<Res>(batch, std::move(req), std::move(cb) );
}

template<typename Res, typename BatchPtr, typename ReqPtr, typename Callback>
void wrocksdb::write_batch_(BatchPtr batch, ReqPtr req, Callback cb)
{
  ::rocksdb::WriteOptions wo;
  wo.sync = req->sync;
  
  auto db = _db1;
  
  if ( db == nullptr )
  {
    if ( cb!=nullptr ) cb( nullptr );
  }
  if ( req->nores || cb == nullptr)
  {
    if ( cb != nullptr )
    {
      auto res = std::make_unique<Res>();
      res->prefix = std::move(req->prefix);
      res->status = common_status::OK ;
      cb( std::move(res) );
    }
    if ( _conf.enable_delayed_write  ) _flow->post([db, wo, batch]() { db->Write( wo, &(*batch));});
    else db->Write( wo, &(*batch));
    
  }
  /*
  else if (  cb == nullptr )
  {
    if ( _conf.enable_delayed_write  ) _flow->post([db, wo, batch]() { db->Write( wo, &(*batch));});
    else db->Write( wo, &(*batch));
  }
  else if ( req->nores )
  {
    auto res = std::make_unique<Res>();
    res->prefix = std::move(req->prefix);
    res->status = common_status::OK ;
    cb( std::move(res) );
    
    if ( _conf.enable_delayed_write  ) _flow->post([db, wo, batch]() { db->Write( wo, &(*batch));});
    else db->Write( wo, &(*batch));
  }*/
  else
  {
    db->Write( wo, &(*batch));
    this->get_<Res>( std::move(req), std::move(cb) );
  }
  
}

template<typename Res, typename ReqPtr, typename Callback>
void wrocksdb::get_(ReqPtr req, Callback cb, bool ignore_if_missing )
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
  std::vector< ::rocksdb::Status> status;
  if ( auto db = _db1 ) status = db->MultiGet( ::rocksdb::ReadOptions(), keys, &resvals);
  else cb(nullptr);

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
    { 
      if ( ignore_if_missing )
        continue; // просто не возвращаем

      // Если ключа нет записываем null
      field.second = "null"; 
    }
    res->fields.push_back( std::move(field) );
  }
  
  cb( std::move(res) );
}

void wrocksdb::set( request::set::ptr req, response::set::handler cb)
{
  auto batch = std::make_shared< ::rocksdb::WriteBatch >();

  for ( const auto& field : req->fields)
  {
    batch->Put( field.first, field.second );
  }
  
  this->write_batch_<response::set>(batch, std::move(req), std::move(cb) );
}

void wrocksdb::get( request::get::ptr req, response::get::handler cb)
{
  this->get_<response::get>( std::move(req), std::move(cb), true );
}

void wrocksdb::has( request::has::ptr req, response::has::handler cb)
{
  this->get_<response::has>( std::move(req), std::move(cb) );
}

void wrocksdb::del( request::del::ptr req, response::del::handler cb) 
{
  
  auto db = _db1;
  if ( db == nullptr ) 
  {
    if (cb!=nullptr) cb(nullptr);
    return;
  }
    
  auto batch = std::make_shared< ::rocksdb::WriteBatch >();
  for ( auto& key : req->fields)
  {
    batch->Delete( key );
  }

  if ( req->nores || cb==nullptr)
  {
    req->nores = true;
    this->write_batch_<response::del>(batch, std::move(req), std::move(cb) );
  }
  else
  {
    // сначало берем, потом удаляем
    this->get_<response::del>( std::move(req), [db, batch, &cb](response::del::ptr res)
    {
      ::rocksdb::Status status = db->Write( ::rocksdb::WriteOptions(), &(*batch));
      cb( std::move(res) );
    }, true);
  }
  
}

void wrocksdb::setnx( request::setnx::ptr req, response::setnx::handler cb) 
{
  this->merge_<merge_mode::setnx, response::setnx>( std::move(req), std::move(cb) );
}

void wrocksdb::inc( request::inc::ptr req, response::inc::handler cb) 
{
  this->merge_<merge_mode::inc, response::inc>( std::move(req), std::move(cb) );
}

void wrocksdb::add( request::add::ptr req, response::add::handler cb) 
{
  this->merge_<merge_mode::add, response::add>( std::move(req), std::move(cb) );
}

void wrocksdb::packed( request::packed::ptr req, response::packed::handler cb)
{  
  this->merge_<merge_mode::packed, response::packed>( std::move(req), std::move(cb) );
}

void wrocksdb::get_updates_since( request::get_updates_since::ptr req, response::get_updates_since::handler cb) 
{
  auto db = _db1;
  if ( db == nullptr ) 
  {
    if (cb!=nullptr) cb(nullptr);
    return;
  }

  auto res = std::make_unique<response::get_updates_since>();
  res->prefix = std::move(req->prefix);

  std::unique_ptr< ::rocksdb::TransactionLogIterator> iter;
  ::rocksdb::Status status = db->GetUpdatesSince(req->seq, &iter, ::rocksdb::TransactionLogIterator::ReadOptions() );
  ::rocksdb::SequenceNumber cur_seq=0;
  if ( status.ok() )
  {
    if ( iter->Valid() )
    {
      res->logs.reserve(req->limit);
      bool first = true;
      while ( iter->Valid() && req->limit-- )
      {
        ::rocksdb::BatchResult batch = iter->GetBatch();
        
        const std::string& data = batch.writeBatchPtr->Data();
        std::string log64;
        log64.reserve(data.size() + data.size()/3);
        encode64(data.begin(), data.end(), std::inserter(log64, log64.end()) );
        res->logs.push_back( log64 );
        cur_seq = batch.sequence;
        if ( first )
        {
          res->seq_first = cur_seq;
          first = false;
        }
        
        iter->Next();
      }
      res->seq_last  = cur_seq;
    }
    else
    {
      res->status = common_status::InvalidSeqNumber;
    }
  }
  
  res->seq_final = db->GetLatestSequenceNumber();
  cb( std::move(res) );  
}

void wrocksdb::get_all_prefixes( request::get_all_prefixes::ptr, response::get_all_prefixes::handler cb)
{
  auto res = std::make_unique<response::get_all_prefixes>();
  res->prefixes.push_back( this->_name );
  cb(std::move(res));
}

void wrocksdb::detach_prefixes( request::detach_prefixes::ptr /*req*/, response::detach_prefixes::handler cb)
{
  std::lock_guard<std::mutex> lk(_mutex);
 
  this->stop_();
  
  if ( !_conf.detach_path.empty() )
  {
    std::string errmsg;
    if ( !move_dir( _conf.path, _conf.detach_path, errmsg ) )
    {
      PREFIXDB_LOG_ERROR("wrocksdb::detach_prefixes " << errmsg);
    }
  };
 
  if ( cb != nullptr )
  {
    auto res = std::make_unique<response::detach_prefixes>();
    cb( std::move(res) );
  }
}

void wrocksdb::attach_prefixes( request::attach_prefixes::ptr /*req*/, response::attach_prefixes::handler cb)
{
  if ( cb!=nullptr ) cb(nullptr);
}

void wrocksdb::range( request::range::ptr req, response::range::handler cb)
{
  auto db = _db1;
  if ( db == nullptr ) 
  {
    if (cb!=nullptr) cb(nullptr);
    return;
  }

  typedef ::rocksdb::Iterator iterator_type;
  
  typedef std::shared_ptr<iterator_type> iterator_ptr;
  auto res=std::make_unique<response::range>();
  res->status = common_status::OK;
  res->prefix = std::move(req->prefix);
  res->fin = false;
  
  ::rocksdb::ReadOptions opt;
  iterator_ptr itr(db->NewIterator(opt));
  if ( itr != nullptr )
  {
    // offset
    itr->Seek( req->from );
    while ( req->offset && itr->Valid() ) 
    {
      req->offset--; itr->Next(); 
    }
    
    while ( req->limit && itr->Valid() )
    {
      field_pair field;
      auto key = itr->key();
      
      field.first.assign(key.data(), key.size() );
      
      if ( !req->to.empty() && field.first > req->to )
        break;
      
      if ( !req->noval )
      {
        auto val = itr->value();
        field.second.assign(val.data(), val.size() );
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

bool wrocksdb::backup()
{
  std::lock_guard<std::mutex> lk(_mutex);
  auto db = _db1;
  if ( db == nullptr ) return false;

  ::rocksdb::Status status;
  status = db->PurgeOldBackups( _conf.backup.depth -1 );
  if ( status.ok() )
  {
  }
  else
  {
    COMMON_LOG_MESSAGE("PurgeOldBackups(" << _conf.backup.depth << ") ERROR for " << _name << ": " << status.ToString() )
  }
  
  status = db->GarbageCollect();
  if ( status.ok() )
  {
    COMMON_LOG_TRACE( "GarbageCollect for " << _name <<  ": " << status.ToString() )
  }
  else
  {
    COMMON_LOG_MESSAGE( "GarbageCollect ERROR for " << _name << ": " << status.ToString() )
  }
  
  status = db->CreateNewBackup();
  if ( status.ok() )
  {
    COMMON_LOG_TRACE("CreateNewBackup for " << _name <<  ": " << status.ToString() )
  }
  else
  {
    COMMON_LOG_MESSAGE("Create Backup ERROR for " << _name << ": " << status.ToString() )
    std::string tmp = _conf.backup.path + ".bak";
    std::string msg;
    delete_dir(tmp, msg);
    move_dir( _conf.backup.path, tmp, msg );
    return false;
  }
  return true;
}

bool wrocksdb::archive(std::string path)
{
  std::lock_guard<std::mutex> lk(_mutex);

  if ( _conf.archive.path.empty() )
    return false;
  path += "/" + _name;

  COMMON_LOG_MESSAGE("Archive for '" << _name << " from " << _conf.backup.path << " ' to " << path)
  std::string error;
  if ( !copy_dir( _conf.backup.path, path, error ) )
  {
    DOMAIN_LOG_ERROR("Archive for '" << _name << "' fail. " << error );
    return true;
  }
  return false;
}

void wrocksdb::compact(const std::string& key)
{
  std::weak_ptr<wrocksdb> wthis = this->shared_from_this();
  _flow->post([wthis, key]()
  {
    if ( auto pthis = wthis.lock() )
    {
      ::rocksdb::Slice skey(key);
      ::rocksdb::Slice skey2(key+"~");
      ::rocksdb::CompactRangeOptions opt;
      opt.exclusive_manual_compaction = true;
      ::rocksdb::Status status = pthis->_db1->CompactRange( opt, &skey, &skey);
      if ( !status.ok())
      {
        PREFIXDB_LOG_ERROR("wrocksdb::compact(" << key << "): " << status.ToString() )
      }
    }
  });
}

void wrocksdb::delay_background( request::delay_background::ptr req, response::delay_background::handler cb) 
{
  auto res = std::make_unique<response::delay_background>();
  ::rocksdb::Status status = _db1->PauseBackgroundWork();
  if ( status.ok() )
  {
    bool force = req->contunue_force;
    _flow->post( std::chrono::seconds(req->delay_timeout_s), [this, force]()
    {
      while ( this->_db1->ContinueBackgroundWork().ok() && force );
    } );
  }
  else
  {
    PREFIXDB_LOG_ERROR("wrocksdb::delay_background: PauseBackgroundWork: " << status.ToString() );
  }
  
  if ( cb != nullptr ) 
    cb( std::move(res) );  
}

void wrocksdb::continue_background( request::continue_background::ptr req, response::continue_background::handler cb) 
{
  auto res = std::make_unique<response::continue_background>();
  while ( this->_db1->ContinueBackgroundWork().ok() && req->force );
  if ( cb != nullptr ) 
    cb( std::move(res) );  
}



}}
