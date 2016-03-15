
#include "rocksdb.hpp"
//#include "persistent_value.hpp"
#include "merge/merge.hpp"
#include "merge/merge_json.hpp"

#include <wfc/logger.hpp>
#include <wfc/json.hpp>
#include <rocksdb/db.h>
#include <rocksdb/write_batch.h>
#include <rocksdb/iterator.h>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <boost/filesystem.hpp>

namespace wamba{ namespace prefixdb {

namespace {
  inline std::string& get_key(std::string& key) {return key;}
  inline std::string& get_key( std::pair<std::string, std::string>& field) {return field.first;}
}

rocksdb::rocksdb( std::string name, const rocksdb_config conf,  db_type* db, restore_db_type* rdb)
  : _name(name)
  , _conf(conf)
  , _db(db)
  , _rdb(rdb)
{
 }

void rocksdb::start( ) 
{
  ::iow::io::timer_options topt;
  topt.start_time = _conf.slave.start_time;
  topt.delay_ms = 1000; /*_conf.slave.pull_timeout_ms;*/
  topt.expires_from_now = true;
  
  // if ( _conf.slave.enabled )
  // if ( _conf.path == "./rocksdb2")
 
 
  if ( auto master = _conf.slave.master )
  {
    if ( _conf.slave.enabled )
    {
      std::weak_ptr<rocksdb> wthis = this->shared_from_this();
      auto hdl = [wthis, master]( ::iow::io::timer::handler_callback handler)
      {
        if ( auto pthis = wthis.lock() )
        {
          // DEBUG_LOG_MESSAGE("rocksdb::rocksdb timer " << pthis->_name)
          // handler();
          auto req = std::make_unique<request::get_updates_since>();
          req->seq = 0;
          req->prefix  = pthis->_name;
          req->limit = 10;
          std::string value;
          ::rocksdb::Status status = pthis->_db->Get( ::rocksdb::ReadOptions(), "~slave-last-sequence-number~", &value);
          if ( status.ok() )
          {
            req->seq = *( reinterpret_cast<const size_t*>( value.data() ) );
          }
          else
          {
            DEBUG_LOG_MESSAGE("Get ~slave-last-sequence-number~ : " << status.ToString() )
          }
          DEBUG_LOG_MESSAGE("request get_updates_since seq=" << req->seq )
          
          master->get_updates_since( std::move(req), [handler, wthis](response::get_updates_since::ptr res)
          {
            bool delay = true;
            if ( auto pthis = wthis.lock() )
            {
              DEBUG_LOG_BEGIN("response get_updates_since res->seq_first=" << res->seq_first << " seq_last=" << res->seq_last << " seq_final=" << res->seq_final )
              if ( res->logs.size() )
              {
                size_t sn = res->seq_last + 1;
                delay = !( sn < res->seq_final );
                ::rocksdb::WriteBatch batch;
                batch.Put("~slave-last-sequence-number~", ::rocksdb::Slice( reinterpret_cast<const char*>(&sn), sizeof(sn) ));
                for (const auto& log : res->logs )
                {
                  //DEBUG_LOG_MESSAGE("\n" << log  )
                  batch.PutLogData( log );
                }
                pthis->_db->Write( ::rocksdb::WriteOptions(), &batch);
              }
              DEBUG_LOG_END("response get_updates_since " << res->logs.size() )
            }
            handler(delay);
          } );
        }
      };
      _conf.slave.timer->start(hdl, topt);
    }
  }
  /*else
  {
    DEBUG_LOG_MESSAGE("rocksdb::start slave disabled " << _conf.slave.target   )
    abort();
  }*/
}

/*
void rocksdb::set_master(std::shared_ptr<iprefixdb> master)
{
  _master = master;
}
*/
void rocksdb::close()
{
  COMMON_LOG_MESSAGE("preffix DB close " << _name)
  _rdb=nullptr;
  _db=nullptr;
}


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
    //std::cout << "rocksdb::merge_ " << json << std::endl;
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

void rocksdb::get_updates_since( request::get_updates_since::ptr req, response::get_updates_since::handler cb) 
{
  DEBUG_LOG_MESSAGE("rocksdb::get_updates_since seq=" << req->seq)
  auto res = std::make_unique<response::get_updates_since>();
  std::unique_ptr< ::rocksdb::TransactionLogIterator> iter;
  ::rocksdb::Status status = _db->GetUpdatesSince(req->seq, &iter, ::rocksdb::TransactionLogIterator::ReadOptions() );
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
        if ( batch.sequence < req->seq )
        {
          std::cout << "batch seq err " << batch.sequence << " < " << req->seq << std::endl;
          // Может быть меньше запрашиваемого
          // Проверить это
          iter->Next();
          continue;
        }
        res->logs.push_back( batch.writeBatchPtr->Data() );
        cur_seq = batch.sequence;
        //std::cout << "cur seq " << cur_seq << std::endl;
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
  res->seq_final = _db->GetLatestSequenceNumber();
  /*
  else
  {
    res->status = common_status::TransactLogError;
    DEBUG_LOG_MESSAGE("rocksdb::get_updates_since " << status.ToString() )
  }*/
  cb( std::move(res) );  
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


namespace {

inline bool backup_status_(const ::rocksdb::Status& s, const request::backup::ptr& req, const response::backup::handler& cb)
{
  if ( s.ok() ) return true;
  if ( cb == nullptr ) return false;
  auto res = std::make_unique< response::backup >();
  res->status = common_status::WriteError;
  if ( !req->nores &&  !req->prefixes.empty() )
  {
    res->status_map.push_back( std::make_pair( std::move(req->prefixes[0]), "false") );
  }
  cb( std::move(res) );
  return false;
}

}


void rocksdb::prebackup_(bool /*compact_range*/)
{
  
}

void rocksdb::backup(bool compact_range)
{
  std::lock_guard<std::mutex> lk(_backup_mutex);
  
  if ( compact_range )
  {
    DEBUG_LOG_BEGIN("CompactRange: " << _name )
    ::rocksdb::Status status = _db->CompactRange( ::rocksdb::CompactRangeOptions(), nullptr, nullptr);
    DEBUG_LOG_END("CompactRange: " << status.ToString() )
  }
  
  {
    ::rocksdb::Status status = _db->GarbageCollect();
    if ( status.ok() )
    {
      DEBUG_LOG_MESSAGE("GarbageCollect for " << _name <<  ": " << status.ToString() )
    }
    else
    {
      COMMON_LOG_MESSAGE("GarbageCollect ERROR for " << _name << ": " << status.ToString() )
    }
  }
  
  ::rocksdb::Status status = _db->CreateNewBackup();
  if ( status.ok() )
  {
    DEBUG_LOG_MESSAGE("CreateNewBackup for " << _name <<  ": " << status.ToString() )
  }
  else
  {
    COMMON_LOG_MESSAGE("Create Backup ERROR for " << _name << ": " << status.ToString() )
  }
  
  status = _db->PurgeOldBackups(5);
  if ( status.ok() )
  {
    DEBUG_LOG_MESSAGE("PurgeOldBackups(5) for " << _name <<  ": " << status.ToString() )
  }
  else
  {
    COMMON_LOG_MESSAGE("PurgeOldBackups(5) ERROR for " << _name << ": " << status.ToString() )
  }
}

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
        fs::copy_file( current, destination / current.filename());
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

void rocksdb::archive(std::string suffix)
{

  DEBUG_LOG_MESSAGE("================== " << suffix << " ==========================")
  std::lock_guard<std::mutex> lk(_backup_mutex);
  if ( _conf.archive_path.empty() )
    return;
  std::string path = _conf.archive_path + suffix + _name;
  //std::string path = suffix + _name;

  COMMON_LOG_MESSAGE("Archive for '" << _name << " from " << _conf.backup_path << " ' to " << path)
  std::string error;
  if ( !copy_dir( _conf.backup_path, path, error ) )
  {
    DOMAIN_LOG_ERROR("Archive for '" << _name << "' fail. " << error );
  }
}

void rocksdb::backup( request::backup::ptr /*req*/, response::backup::handler cb) 
{
  if (cb!=nullptr)
    cb(nullptr);
  /*
  if ( req->compact_range )
  {
    ::rocksdb::Status status = _db->CompactRange( ::rocksdb::CompactRangeOptions(), nullptr, nullptr);
    DEBUG_LOG_MESSAGE("CompactRange: " << status.ToString() )
  }
  ::rocksdb::Status status = _db->CreateNewBackup();
  DEBUG_LOG_MESSAGE("CreateNewBackup: " << status.ToString() )
  if ( cb != nullptr )
  {
    auto res = std::make_unique<response::backup>();
    res->status = status.ok() ? common_status::OK : common_status::WriteError;
    cb( std::move(res) );
  }
  */
  /*
  typedef ::rocksdb::BackupEngine backup_engine;
  backup_engine* engine;
  ::rocksdb::BackupableDBOptions opt(req->path);
  ::rocksdb::Status s = ::rocksdb::BackupEngine::Open( ::rocksdb::Env::Default(), opt, &engine);
  if ( !backup_status_(s, req, cb) )
    return;
  std::unique_ptr<backup_engine> ptr(engine);
  ptr->CreateNewBackup( _db.get() );
  */
}

void rocksdb::restore() 
{
  ::rocksdb::SequenceNumber seq_number = 0/*_db->GetLatestSequenceNumber()*/;
  std::unique_ptr< ::rocksdb::TransactionLogIterator> iter;
  
  ::rocksdb::Status status = _db->GetUpdatesSince(seq_number, &iter, ::rocksdb::TransactionLogIterator::ReadOptions() );
  
  std::cout << "Replication " << status.ToString() << ": " << seq_number << std::endl;
  while (iter->Valid() )
  {
    ::rocksdb::BatchResult batch = iter->GetBatch();
    std::string ser = batch.writeBatchPtr->Data();
    std::cout << "LOG sequence=" << batch.sequence << ":" << ser << std::endl;
    /*::rocksdb::WriteBatch wb;
    wb.PutLogData();*/
    //_db->GetEnv()->WriteStringToFile();
    //batch.seq_number
    
    iter->Next();
  }
  
  /*
  virtual Status GetUpdatesSince(
      SequenceNumber seq_number, unique_ptr<TransactionLogIterator>* iter,
      const TransactionLogIterator::ReadOptions&
          read_options = TransactionLogIterator::ReadOptions()) = 0;
          */

  //auto path = _conf.path;
  /*
  std::vector< ::rocksdb::BackupInfo > backup_info;
  _rdb->GetBackupInfo(&backup_info);
  for (const auto bi : backup_info)
  {
    ::rocksdb::Status status = _rdb->RestoreDBFromBackup( bi.backup_id, path, path, ::rocksdb::RestoreOptions());
    DEBUG_LOG_MESSAGE("RestoreDBFromBackup (" <<bi.backup_id << "): "  << status.ToString() << " to " << path );
    status = _rdb->DeleteBackup( bi.backup_id );
    DEBUG_LOG_MESSAGE("DeleteBackup: " << status.ToString() )
  }
  */
  
  /*
  DEBUG_LOG_BEGIN("RestoreDBFromLatestBackup... " )
  ::rocksdb::Status status = _rdb->RestoreDBFromLatestBackup(path, path);
  DEBUG_LOG_END("RestoreDBFromLatestBackup: " << status.ToString() )
  */
  //status = _rdb->PurgeOldBackups(1);
  //DEBUG_LOG_MESSAGE("PurgeOldBackups: " << status.ToString() )
  
}


void rocksdb::restore( request::restore::ptr req, response::restore::handler cb) 
{
  ::rocksdb::Status status = _rdb->RestoreDBFromLatestBackup(req->path, req->path);
  DEBUG_LOG_MESSAGE("RestoreDBFromLatestBackup: " << status.ToString() )
  if ( cb != nullptr )
  {
    auto res = std::make_unique<response::restore>();
    res->status = status.ok() ? common_status::OK : common_status::WriteError;
    cb( std::move(res) );
  }
}

}}
