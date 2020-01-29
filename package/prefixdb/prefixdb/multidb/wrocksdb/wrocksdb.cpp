#include "wrocksdb.hpp"
#include "wrocksdb_slave.hpp"
#include "merge/merge.hpp"
#include "merge/merge_json.hpp"
#include "../aux/base64.hpp"
#include "merge/packed_params_json.hpp"
#include "merge/add_params_json.hpp"
#include "merge/inc_params_json.hpp"
#include "wrocksdb_initial.hpp"

#include <prefixdb/logger.hpp>
#include <wfc/json.hpp>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <chrono>
#include <string>
#include <functional>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlong-long"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <rocksdb/db.h>
#include <rocksdb/write_batch.h>
#include <rocksdb/utilities/backupable_db.h>
#include <rocksdb/iterator.h>
#include <rocksdb/utilities/db_ttl.h>
#pragma GCC diagnostic pop


#include "../aux/copy_dir.hpp"


namespace wamba{ namespace prefixdb {

namespace 
{
  inline std::string& get_key(std::string& key) {return key;}
  inline std::string& get_key( std::pair<std::string, std::string>& field) {return field.first;}
}

wrocksdb::wrocksdb( std::string name, const db_config conf,  db_type* db, backup_type* bk)
  : _name(name)
  , _conf(conf)
  , _db(db)
  , _backup(bk)
{
  _workflow = conf.args.timers_workflow;
  _write_workflow = conf.args.write_workflow;
  if ( _write_workflow == nullptr )
    _write_workflow = _workflow;
}


void wrocksdb::start( ) 
{
  if ( _conf.initial_load.enabled )
  {
    using namespace std::placeholders;
    auto opt = _conf.initial_load;
    opt.local = this->shared_from_this();
    _initial = std::make_shared<wrocksdb_initial>(_name, opt, *_db);
    _initial->load( std::bind( &wrocksdb::slave_start_, this, _1) );
  }
  else
    this->slave_start_(0);
}

void wrocksdb::slave_start_( size_t seq_num ) 
{
  std::lock_guard<std::mutex> lk(_mutex);
  if ( _conf.slave.enabled )
  {
    _slave = std::make_shared<wrocksdb_slave>(_name, _conf.path, _conf.slave, *_db);
    _slave->start(seq_num);
  }
}

void wrocksdb::stop_()
{
  PREFIXDB_LOG_MESSAGE("preffix DB stop for prefix=" << _name)
  if ( _slave!=nullptr ) 
    _slave->stop();
  if ( _initial!=nullptr)
    _initial->stop();
  _slave = nullptr;
  _initial = nullptr;
  _db=nullptr;
}

void wrocksdb::stop()
{
  std::lock_guard<std::mutex> lk(_mutex);
  this->stop_();

}

namespace{
  template<typename Json, typename Res, typename ReqPtr, typename Callback>
  bool check_params(ReqPtr& req, Callback& cb)
  {
    typedef Json params_json;
    typedef typename params_json::target params_t;
    typedef typename params_json::serializer serializer;
    bool noerr = true;
    params_t pkg;

    ::wfc::json::json_error e;
    for (const auto& field : req->fields )
    {
      serializer()(pkg, field.second.begin(), field.second.end(), &e );
      if ( e )
      {
        PREFIXDB_LOG_ERROR( "JSONRPC params error: " << wfc::json::strerror::message_trace( e, field.second.begin(), field.second.end() ) );
        noerr=false; 
        break;
      }
    }

    if ( !noerr && cb!=nullptr)
    {
      auto res = std::make_unique<Res>();
      res->status = common_status::InvalidFieldValue;
      cb( std::move(res) );
    }
    return noerr;
  }
}

bool wrocksdb::check_inc_(request::inc::ptr& req, response::inc::handler& cb)
{
  if ( !this->_conf.check_incoming_merge_json )
    return true;

  return check_params<inc_params_json, response::inc>(req, cb);

}

bool wrocksdb::check_add_(request::add::ptr& req, response::add::handler& cb)
{
  if ( !this->_conf.check_incoming_merge_json )
    return true;

  return check_params<add_params_json, response::add>(req, cb);
}

bool wrocksdb::check_packed_(request::packed::ptr& req, response::packed::handler& cb)
{
  if ( !this->_conf.check_incoming_merge_json )
    return true;

  return check_params<packed_params_json, response::packed>(req, cb);
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

namespace {

template<typename Res, typename ReqPtr>
std::unique_ptr<Res> create_result_ok(ReqPtr& req)
{
  auto res = std::make_unique<Res>();
  res->prefix = std::move(req->prefix);
  res->status = common_status::OK ;
  return std::move(res);
}

}

template<typename Res, typename BatchPtr, typename ReqPtr, typename Callback>
void wrocksdb::write_batch_(BatchPtr batch, ReqPtr req, Callback cb)
{
  ::rocksdb::WriteOptions wo;
  wo.sync = req->sync;
  
  auto db = _db;
  
  if ( db == nullptr )
  {
    if ( cb!=nullptr ) 
      cb( nullptr );
  }

  if ( req->nores || cb == nullptr)
  {
    // Если не нужен результат и не нужна синхронная запись, то сначала отправляем ответ
    if ( cb != nullptr && !req->sync )
      cb( create_result_ok<Res>(req) );

    // Если включена запись через очередь 
    if ( !req->sync && _conf.enable_delayed_write ) 
    {
      if ( !req->sync || cb==nullptr ) 
      {
        // Если не нужна синхронная запись, от ответ уже отправили
        _write_workflow->post([db, wo, batch]() { db->Write( wo, &(*batch)); }, nullptr);
      }
      else
      {
        auto preq = std::make_shared<ReqPtr>( std::move(req) );
        _write_workflow->post([db, wo, batch, preq, cb]() 
        {
          db->Write( wo, &(*batch)); 
          cb( create_result_ok<Res>( *preq) );
        }, [cb](){ cb(nullptr);});
      }
    }
    else 
    {
      db->Write( wo, &(*batch));
      if ( cb != nullptr && req->sync )
        cb( create_result_ok<Res>(req) );
    }
  }
  else
  {
    db->Write( wo, &(*batch));
    this->get_<Res>( std::move(req), std::move(cb), false );
  }
  
}

wrocksdb::snapshot_ptr wrocksdb::find_snapshot_(size_t id) const
{
  if ( id == 0 )
    return nullptr;
  std::lock_guard<std::mutex> lk(_mutex);
  auto itr = _snapshot_map.find(id);
  if ( itr!=_snapshot_map.end() )
    return itr->second;
  return nullptr;
}

std::string wrocksdb::get_property(const std::string& name) const 
{
  std::string res;
  _db->GetProperty(name, &res);
  return res;
}

size_t wrocksdb::create_snapshot_(size_t *seq_num)
{
  auto db = _db;
  if (db==nullptr) return 0;
  
  snapshot_ptr ss = db->GetSnapshot();
  if ( ss == nullptr )
  {
    PREFIXDB_LOG_ERROR("Create snapshot FAIL") 
    return 0;
  }

  if ( seq_num!=nullptr )
    *seq_num = ss->GetSequenceNumber();
  
  size_t size = 0;
  {
    std::lock_guard<std::mutex> lk(_mutex);
    ++_snapshot_counter;
    _snapshot_map.emplace(_snapshot_counter, ss);
    size = _snapshot_map.size();
  }
  PREFIXDB_LOG_MESSAGE( _name << ". Create snapshot №" << _snapshot_counter << ", snapshot count " << size << " rocksdb::DB::kNumSnapshots=" 
                      << this->get_property(rocksdb::DB::Properties::kNumSnapshots) )
  return _snapshot_counter;
}

bool wrocksdb::release_snapshot_(size_t id)
{
  snapshot_ptr ss = nullptr;
  {
    std::lock_guard<std::mutex> lk(_mutex);
    auto itr = _snapshot_map.find(id);
    if ( itr==_snapshot_map.end() )
      return false;
    ss = itr->second;
    _snapshot_map.erase(itr);
  }
  if ( auto db = _db )
  {
    PREFIXDB_LOG_MESSAGE( "Release snapshot №" << id << " total=" << _snapshot_map.size() )  
    db->ReleaseSnapshot(ss);
  }
  return true;
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
  ::rocksdb::ReadOptions ro;
  
  if ( req->snapshot!=0 )
  {
    ro.snapshot = this->find_snapshot_(req->snapshot);
    if ( ro.snapshot == nullptr )
    {
      auto res = std::make_unique<response_type>();
      res->prefix = std::move(req->prefix);
      res->status = common_status::SnapshotNotFound;
      cb( std::move(res) );
      return;
    }
  }
  
  if ( auto db = _db ) 
    status = db->MultiGet( ro, keys, &resvals);
  else { cb(nullptr); return;}

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
    { // Если ключ существует
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
  this->get_<response::has>( std::move(req), std::move(cb), false );
}

void wrocksdb::del( request::del::ptr req, response::del::handler cb) 
{
  auto db = _db;
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
  if ( this->check_add_(req, cb) )
    this->merge_<merge_mode::add, response::add>( std::move(req), std::move(cb) );
}

void wrocksdb::packed( request::packed::ptr req, response::packed::handler cb)
{
  if ( this->check_packed_(req, cb) )
    this->merge_<merge_mode::packed, response::packed>( std::move(req), std::move(cb) );
}

void wrocksdb::get_updates_since( request::get_updates_since::ptr req, response::get_updates_since::handler cb) 
{
  auto db = _db;
  if ( db == nullptr ) 
  {
    if (cb!=nullptr) 
      cb(nullptr);
    return;
  }

  auto res = std::make_unique<response::get_updates_since>();
  res->prefix = std::move(req->prefix);

  std::unique_ptr< ::rocksdb::TransactionLogIterator> iter;
  ::rocksdb::TransactionLogIterator::ReadOptions ro;
  ro.verify_checksums_ = false; // TODO: опционально
  ::rocksdb::Status status = db->GetUpdatesSince(req->seq, &iter, ro );
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
  auto db = _db;
  if ( db.use_count() > 2 )
  {
    db.reset();
    std::weak_ptr<wrocksdb> wthis = this->shared_from_this();
    _workflow->safe_post(
      std::chrono::milliseconds(100),
      [wthis, cb](){
      if (auto pthis = wthis.lock() )
        pthis->detach_prefixes(std::make_unique<request::detach_prefixes>(), cb);
    });
    return;
  }
 
  this->stop_();

  PREFIXDB_LOG_MESSAGE("Detach Prefix " << _name)
  
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
  PREFIXDB_LOG_MESSAGE("Attach Prefixes " << _name)
  if ( cb!=nullptr ) cb(nullptr);
}

void wrocksdb::range( request::range::ptr req, response::range::handler cb)
{
  auto db = _db;
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
    if ( itr->Valid() && req->beg == false )
      itr->Next(); 
    
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

void wrocksdb::compact_prefix( request::compact_prefix::ptr req, response::compact_prefix::handler cb) 
{
  auto preq = std::make_shared<request::compact_prefix>( std::move(*req) );
  std::thread([preq, this, cb](){
    PREFIXDB_LOG_BEGIN("Manual Compaction " << this->_name);
    bool compact_res = false;
    if (preq->from.empty() && preq->to.empty() )
    {
      compact_res = this->compact();
    }
    else
    {
      typedef ::rocksdb::Slice slice_type;
      typedef std::unique_ptr<slice_type> slice_ptr;
      slice_ptr fromptr = nullptr;
      slice_ptr toptr = nullptr;
      if ( !preq->from.empty() )
        fromptr = std::make_unique<slice_type>(preq->from);
      if ( !preq->to.empty() )
        toptr = std::make_unique<slice_type>(preq->to);
      
      if (auto db = _db)
      {
        ::rocksdb::CompactRangeOptions opt;
        ::rocksdb::Status status = db->CompactRange( opt, fromptr.get(), toptr.get() );
        compact_res = status.ok();
      }
    }
    
    auto res = std::make_unique<response::compact_prefix>();
    res->prefix = std::move(preq->prefix);
    res->status = compact_res ? common_status::OK : common_status::CompactFail;
    cb( std::move(res) );
    PREFIXDB_LOG_END("Manual Compaction " << this->_name);
  }).detach(); // thread
}

void wrocksdb::create_snapshot( request::create_snapshot::ptr req, response::create_snapshot::handler cb) 
{
  auto res = std::make_unique<response::create_snapshot>();
  res->prefix = std::move(req->prefix);
  if ( auto id = this->create_snapshot_( &(res->last_seq) ) )
  {
    res->snapshot = id;
    if ( 0 != req->release_timeout_s )
    {
      _workflow->safe_post(
        std::chrono::seconds(req->release_timeout_s),
        [this, id]()
        {
          this->release_snapshot_(id);
        }
      );
    }
  }
  else
  {
    res->status = common_status::CreateSnapshotFail;
  }
  cb( std::move(res) );
}

void wrocksdb::release_snapshot( request::release_snapshot::ptr req, response::release_snapshot::handler cb) 
{
  bool status = this->release_snapshot_(req->snapshot);
  if (cb!=nullptr)
  {
    auto res = std::make_unique<response::release_snapshot>();
    res->prefix = std::move(req->prefix);
    if (!status)
      res->status = common_status::SnapshotNotFound;
    cb( std::move(res) );
  }
  
}


bool wrocksdb::backup()
{
  std::lock_guard<std::mutex> lk(_mutex);
  
  if ( _backup == nullptr ) return false;

  ::rocksdb::Status status;
  status = _backup->PurgeOldBackups( _conf.backup.depth - 1 );
  if ( status.ok() )
  {
  }
  else
  {
    PREFIXDB_LOG_ERROR("PurgeOldBackups(" << _conf.backup.depth << ") ERROR for " << _name << ": " << status.ToString() )
  }
  
  status = _backup->GarbageCollect();
  if ( status.ok() )
  {
    PREFIXDB_LOG_TRACE( "GarbageCollect for " << _name <<  ": " << status.ToString() )
  }
  else
  {
    PREFIXDB_LOG_ERROR( "GarbageCollect ERROR for " << _name << ": " << status.ToString() )
  }
  
  PREFIXDB_LOG_BEGIN("CreateNewBackup...")
  if (auto db = _db)
  {
    size_t progress = 0;
    status = _backup->CreateNewBackup( db.get(), true, 
      [progress]() mutable { PREFIXDB_LOG_PROGRESS("CreateNewBackup...." << std::string(progress++, '.') ) });
  }
  
  if ( status.ok() )
  {
    PREFIXDB_LOG_TRACE("CreateNewBackup for " << _name <<  ": " << status.ToString() )
  }
  else
  {
    PREFIXDB_LOG_ERROR("Create Backup ERROR for " << _name << ": " << status.ToString() )
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

  PREFIXDB_LOG_MESSAGE("Archive for '" << _name << " from " << _conf.backup.path << " ' to " << path)
  std::string error;
  if ( !copy_dir( _conf.backup.path, path, error ) )
  {
    PREFIXDB_LOG_ERROR("Archive for '" << _name << "' fail. " << error );
    return true;
  }
  return false;
}

bool wrocksdb::compact()
{
  bool result  = false;
  if ( auto db = _db )
  {
    ::rocksdb::CompactRangeOptions opt;
    opt.exclusive_manual_compaction = true;
    ::rocksdb::Status status = db->CompactRange( opt, nullptr, nullptr );
    result = status.ok();
    if ( !result ) 
    {
      PREFIXDB_LOG_ERROR( "wrocksdb::compact error: " << status.ToString() )
    }
  }
  
  return result;
}

void wrocksdb::delay_background( request::delay_background::ptr req, response::delay_background::handler cb) 
{
  auto db = _db;
  if (db == nullptr)
  {
    if (cb!=nullptr) cb(nullptr);
    return;
  }
  auto res = std::make_unique<response::delay_background>();
  ::rocksdb::Status status = db->PauseBackgroundWork();
  if ( status.ok() )
  {
    PREFIXDB_LOG_BEGIN("Delay Background. kIsWriteStopped==" << this->get_property(rocksdb::DB::Properties::kIsWriteStopped) )
    bool force = req->contunue_force;
    std::weak_ptr<db_type> wdb = db;
    auto cb_fun = [wdb, force]()
    {
      if ( auto pdb = wdb.lock() )
      {
        while ( pdb->ContinueBackgroundWork().ok() && force );
        std::string val;
        pdb->GetProperty(rocksdb::DB::Properties::kIsWriteStopped, &val);
        PREFIXDB_LOG_END("Delay Background [ContinueBackgroundWork]. kIsWriteStopped==" << val )
      }
    };
    _workflow->safe_post( std::chrono::seconds(req->delay_timeout_s), cb_fun);
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
  if ( auto db = _db )
  {
    ::rocksdb::Status status;
    do
    {
      status = db->ContinueBackgroundWork();
      PREFIXDB_LOG_MESSAGE("ContinueBackgroundWork: " << status.ToString())
    }
    while ( status.ok() && req->force );
  }
  PREFIXDB_LOG_END("Delay Background [ContinueBackgroundWork]. kIsWriteStopped==" << this->get_property(rocksdb::DB::Properties::kIsWriteStopped) )
  if ( cb != nullptr ) 
    cb( std::move(res) );  
}




}}
