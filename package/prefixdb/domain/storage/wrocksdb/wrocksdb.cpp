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
  _slave = std::make_shared<wrocksdb_slave>(name, conf.slave, *db);
}


void wrocksdb::start( ) 
{
  std::lock_guard<std::mutex> lk(_mutex);
  _slave->start();
}



void wrocksdb::stop_()
{
  PREFIXDB_LOG_MESSAGE("preffix DB stop for prefix=" << _name)
  _slave->stop();
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
    batch.Merge(f.first, json);
  }
  
  this->write_batch_<Res>(batch, std::move(req), std::move(cb) );
}

template<typename Res, typename Batch, typename ReqPtr, typename Callback>
void wrocksdb::write_batch_(Batch& batch, ReqPtr req, Callback cb)
{
  ::rocksdb::WriteOptions wo;
  wo.sync = req->sync;
  
  
  ::rocksdb::Status status;
  if ( auto db = _db1 ) status = db->Write( ::rocksdb::WriteOptions(), &batch);
  else return;

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
void wrocksdb::get_(ReqPtr req, Callback cb)
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
    { // Если ключа нет записывам null
      field.second = "null";
    }
    res->fields.push_back( std::move(field) );
  }
  
  cb( std::move(res) );
}

void wrocksdb::set( request::set::ptr req, response::set::handler cb)
{
  ::rocksdb::WriteBatch batch;

  for ( const auto& field : req->fields)
  {
    batch.Put( field.first, field.second );
  }
  
  this->write_batch_<response::set>(batch, std::move(req), std::move(cb) );
}

void wrocksdb::get( request::get::ptr req, response::get::handler cb)
{
  this->get_<response::get>( std::move(req), std::move(cb) );
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
    
  ::rocksdb::WriteBatch batch;
  for ( auto& key : req->fields)
  {
    batch.Delete( key );
  }

  if ( req->nores || cb==nullptr)
  {
    ::rocksdb::Status status = db->Write( ::rocksdb::WriteOptions(), &batch);
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
    this->get_<response::del>( std::move(req), [db, &batch, &cb](response::del::ptr res)
    {
      ::rocksdb::Status status = db->Write( ::rocksdb::WriteOptions(), &batch);
      res->status = status.ok() ? common_status::OK : common_status::WriteError;
      cb( std::move(res) );
    });
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
        log64.reserve(512);
        encode64(data.begin(), data.end(), std::inserter(log64, log64.end()) );
        res->logs.push_back( log64 );
        cur_seq = batch.sequence;
        if ( first )
        {
          DEBUG_LOG_MESSAGE("rocksdb::get_updates_since first_seq=" << batch.sequence)
          res->seq_first = cur_seq;
          first = false;
        }
        
        iter->Next();
      }
      res->seq_last  = cur_seq;
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

void wrocksdb::range( request::range::ptr req, response::range::handler cb)
{
  auto db = _db1;
  if ( db == nullptr ) 
  {
    if (cb!=nullptr) cb(nullptr);
    return;
  }

  DEBUG_LOG_MESSAGE("range from '" << req->from << " to '" << req->to )
  typedef ::rocksdb::Iterator iterator_type;
  typedef ::rocksdb::Slice slice_type;
  
  typedef std::shared_ptr<iterator_type> iterator_ptr;
  auto res=std::make_unique<response::range>();
  res->status = common_status::OK;
  res->prefix = std::move(req->prefix);
  res->fin = false;
  
  ::rocksdb::ReadOptions opt;
  iterator_ptr itr(db->NewIterator(opt));
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

bool wrocksdb::backup()
{
  std::lock_guard<std::mutex> lk(_mutex);
  auto db = _db1;
  if ( db == nullptr ) return false;

  
  /*
  if ( _conf.compact_before_backup )
  {
    DEBUG_LOG_BEGIN("CompactRange: " << _name )
    ::rocksdb::Status status = _db->CompactRange( ::rocksdb::CompactRangeOptions(), nullptr, nullptr);
    DEBUG_LOG_END("CompactRange: " << status.ToString() )
  }
  */
  
  {
    ::rocksdb::Status status = db->GarbageCollect();
    if ( status.ok() )
    {
      DEBUG_LOG_MESSAGE( "GarbageCollect for " << _name <<  ": " << status.ToString() )
    }
    else
    {
      COMMON_LOG_MESSAGE( "GarbageCollect ERROR for " << _name << ": " << status.ToString() )
    }
  }
  
  ::rocksdb::Status status = db->CreateNewBackup();
  if ( status.ok() )
  {
    DEBUG_LOG_MESSAGE("CreateNewBackup for " << _name <<  ": " << status.ToString() )
  }
  else
  {
    COMMON_LOG_MESSAGE("Create Backup ERROR for " << _name << ": " << status.ToString() )
  }
  
  status = db->PurgeOldBackups( _conf.backup.depth );
  if ( status.ok() )
  {
    DEBUG_LOG_MESSAGE("PurgeOldBackups(5) for " << _name <<  ": " << status.ToString() )
  }
  else
  {
    COMMON_LOG_MESSAGE("PurgeOldBackups(" << _conf.backup.depth << ") ERROR for " << _name << ": " << status.ToString() )
    return false;
  }
  return true;
}

/*
namespace 
{
  inline bool copy_dir(
    boost::filesystem::path const & source,
    boost::filesystem::path const & destination,
    std::string& message
  )
  {
    namespace fs = boost::filesystem;
  
    try
    {
      // Check whether the function call is valid
      if( !fs::exists(source) || !fs::is_directory(source) )
      {
        std::stringstream ss;
        ss << "Source directory " << source.string() << " does not exist or is not a directory." << '\n';
        message = ss.str();
        return false;
      }
      
      if ( fs::exists(destination) )
      {
        std::stringstream ss;
        ss << "Destination directory " << destination.string() << " already exists." << '\n';
        message = ss.str();
        return false;
      }
      
      // Create the destination directory
      if( !fs::create_directory(destination) )
      {
        std::stringstream ss;
        ss << "Unable to create destination directory " << destination.string() << '\n';
        message = ss.str();
        return false;
      }
    }
    catch(fs::filesystem_error const & e)
    {
      std::stringstream ss;
      ss << e.what() << '\n';
      message = ss.str();
      return false;
    }
    
    // Iterate through the source directory
    for( fs::directory_iterator file(source); file != fs::directory_iterator(); ++file) try
    {
      fs::path current(file->path());
      if( fs::is_directory(current) )
      {
        // Found directory: Recursion
        if( !copy_dir(current, destination / current.filename(), message) )
          return false;
      }
      else
      {
        // Found file: Copy
        ::boost::system::error_code ec;
        ::boost::filesystem::copy_file( current, destination / current.filename(), ec);
        if (ec)
        {
          // TODO: Ошибка
        }
      }
    }
    catch(const fs::filesystem_error& e)
    {
      std::stringstream ss;
      ss << e.what() << '\n';
      message = ss.str();
      return false;
    }
    return true;
  }

  inline bool copy_dir(const std::string& from, const std::string& to, std::string& message)
  {
    return copy_dir( ::boost::filesystem::path(from),  ::boost::filesystem::path(to), message);
  }
}
*/

bool wrocksdb::archive(std::string path)
{
  DEBUG_LOG_MESSAGE("================== " << path << " ==========================")
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


}}
