
#include "rocksdb.hpp"
#include "merge/merge.hpp"
#include "merge/merge_json.hpp"

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

#include <boost/filesystem.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>


namespace wamba{ namespace prefixdb {

namespace 
{
  inline std::string& get_key(std::string& key) {return key;}
  inline std::string& get_key( std::pair<std::string, std::string>& field) {return field.first;}
  
  template<typename I>
  inline std::vector<char> decode64(I beg, I end) 
  {
    using namespace boost::archive::iterators;
    using iterator = transform_width< binary_from_base64<std::string::const_iterator>, 8, 6 >;
    //return std::vector<char>( iterator(beg), iterator(end) );
    
    return ::boost::algorithm::trim_right_copy_if(std::vector<char>(iterator(beg), iterator(end)), [](char c) { return c == '\0'; });
  }

  template<typename I>
  inline std::string encode64(I beg, I end) 
  {
      using namespace boost::archive::iterators;
      using iterator = base64_from_binary<transform_width< I, 6, 8>>;
      std::string res;
      res.assign(iterator( beg) , iterator(end));
      return res.append((3 - std::distance(beg,end) % 3) % 3, '=');
  }
}

rocksdb::rocksdb( std::string name, const rocksdb_config conf,  db_type* db)
  : _name(name)
  , _conf(conf)
  , _db(db)
  , _slave_timer_id(-1)
  
{
}


void rocksdb::start( ) 
{
  if ( _conf.slave.master!=nullptr && _conf.slave.enabled )
  {
    this->create_slave_timer_();
  }
}



void rocksdb::close()
{
  COMMON_LOG_MESSAGE("preffix DB close " << _name)
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

void rocksdb::setnx( request::setnx::ptr req, response::setnx::handler cb) 
{
  this->merge_<merge_mode::setnx, response::setnx>( std::move(req), std::move(cb) );
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
  auto res = std::make_unique<response::get_updates_since>();
  res->prefix = std::move(req->prefix);
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
          DEBUG_LOG_MESSAGE("batch seq err " << batch.sequence << " < " << req->seq );
          // Может быть меньше запрашиваемого
          // Проверить это
          iter->Next();
          continue;
        }
        
        const std::string& data = batch.writeBatchPtr->Data();
        res->logs.push_back( encode64(data.begin(), data.end() ) );
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
  res->seq_final = _db->GetLatestSequenceNumber();
  cb( std::move(res) );  
}

void rocksdb::create_slave_timer_()
{
  auto preq = std::make_shared<request::get_updates_since>();
  preq->seq = 0;
  preq->prefix  = this->_name;
  preq->limit = this->_conf.slave.log_limit_per_req;
  
  std::string value;
  if ( _name == "test" )
  {
              ::rocksdb::WriteOptions wo;
          wo.sync = true;

    _db->Delete(wo, "~slave-last-sequence-number~");
  }
  
  ::rocksdb::Status status = this->_db->Get( ::rocksdb::ReadOptions(), "~slave-last-sequence-number~", &value);
  if ( status.ok() )
  {
    preq->seq = *( reinterpret_cast<const size_t*>( value.data() ) );
    COMMON_LOG_MESSAGE(_name << " ~slave-last-sequence-number~ " << value )
  }
  else
  {
    preq->seq = _db->GetLatestSequenceNumber() + 1;
  }
  DEBUG_LOG_MESSAGE("request get_updates_since seq=" << preq->seq )
  auto pdiff = std::make_shared< std::atomic_size_t>(0);
  if ( this->_conf.slave.wrn_log_diff_seq !=0 )
  {
    // TODO: сделать остановку таймера
    auto maxdiff = this->_conf.slave.wrn_log_diff_seq;
    _conf.slave.timer->create_timer(
      std::chrono::seconds(1),
      [pdiff, maxdiff]()->bool
      {
        if ( *pdiff > maxdiff )
        {
          PREFIXDB_LOG_WARNING("Slave replication too big difference " << *pdiff << "(max:" << maxdiff << ")");
        }
        return true;
      }
    );
  }

  _slave_timer_id = _conf.slave.timer->create_requester<request::get_updates_since, response::get_updates_since>
  (
    _conf.slave.start_time,
    std::chrono::milliseconds(_conf.slave.pull_timeout_ms),
    _conf.slave.master,
    &iprefixdb::get_updates_since,
    [this, preq, pdiff](response::get_updates_since::ptr res) -> request::get_updates_since::ptr
    {
      if ( res == nullptr )
      {
        DEBUG_LOG_MESSAGE("NEW QUERY seq = " << preq->seq );
        return std::make_unique<request::get_updates_since>(*preq);
      }
      
      if ( res->logs.empty() )
        return nullptr;

      if ( preq->seq!=0  )
      {
        auto diff = res->seq_first - preq->seq;
        if ( diff > this->_conf.slave.acceptable_loss_seq )
        {
          ::rocksdb::WriteOptions wo;
          wo.sync = true;
          _db->Delete(wo, "~slave-last-sequence-number~");// временно
          DOMAIN_LOG_FATAL( _name << " Slave not acceptable loss sequence: " << diff << " request segment=" << preq->seq << " response=" << res->seq_first)
          ::wfc_abort("Slave replication error");
          abort();
          return nullptr;
        }
      }
      
      *pdiff = res->seq_final - res->seq_first ;

      auto count = res->seq_first;
      for (const auto& log : res->logs )
      {
        try
        {
          auto binlog = decode64(log.begin(), log.end());
          ++count;
          this->_reader.parse( binlog );
        }
        catch(...)
        {
          res->seq_last = count - 1;
          this->_reader.reset();
          PREFIXDB_LOG_ERROR("attempt to decode a value not in base64 char set: [" << log << "]" )
          break;
        }
      }

      auto batch = _reader.detach();
      size_t sn = res->seq_last + 1;
      batch->Put("~slave-last-sequence-number~", ::rocksdb::Slice( reinterpret_cast<const char*>(&sn), sizeof(sn) ));
      COMMON_LOG_MESSAGE(_name << " PUT ~slave-last-sequence-number~ " << sn ) 
      this->_db->Write( ::rocksdb::WriteOptions(), batch.get() );

      preq->seq = sn;
      if ( res->seq_last == res->seq_final )
        return nullptr;
      
      return std::make_unique<request::get_updates_since>(*preq);
    }
  );
}


void rocksdb::get_all_prefixes( request::get_all_prefixes::ptr, response::get_all_prefixes::handler cb)
{
  auto res = std::make_unique<response::get_all_prefixes>();
  res->prefixes.push_back( this->_name );
  cb(std::move(res));
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

bool rocksdb::backup()
{
  std::lock_guard<std::mutex> lk(_backup_mutex);
  
  if ( _conf.compact_before_backup )
  {
    DEBUG_LOG_BEGIN("CompactRange: " << _name )
    ::rocksdb::Status status = _db->CompactRange( ::rocksdb::CompactRangeOptions(), nullptr, nullptr);
    DEBUG_LOG_END("CompactRange: " << status.ToString() )
  }
  
  {
    ::rocksdb::Status status = _db->GarbageCollect();
    if ( status.ok() )
    {
      DEBUG_LOG_MESSAGE( "GarbageCollect for " << _name <<  ": " << status.ToString() )
    }
    else
    {
      COMMON_LOG_MESSAGE( "GarbageCollect ERROR for " << _name << ": " << status.ToString() )
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
    return false;
  }
  return true;
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

bool rocksdb::archive(std::string path)
{
  DEBUG_LOG_MESSAGE("================== " << path << " ==========================")
  std::lock_guard<std::mutex> lk(_backup_mutex);
  if ( _conf.archive_path.empty() )
    return false;
  path += "/" + _name;

  COMMON_LOG_MESSAGE("Archive for '" << _name << " from " << _conf.backup_path << " ' to " << path)
  std::string error;
  if ( !copy_dir( _conf.backup_path, path, error ) )
  {
    DOMAIN_LOG_ERROR("Archive for '" << _name << "' fail. " << error );
    return true;
  }
  return false;
}


rocksdb_restore::rocksdb_restore(std::string name, const rocksdb_config conf, restore_db_type* rdb)
  : _name(name)
  , _conf(conf)
  , _rdb(rdb)
{
  
}
bool rocksdb_restore::restore() 
{
  COMMON_LOG_BEGIN("Restore for " << _name << " to " << _conf.path << " from " << _conf.restore_path )
  ::rocksdb::Status status = _rdb->RestoreDBFromLatestBackup( _conf.path, _conf.path, ::rocksdb::RestoreOptions() );
  COMMON_LOG_END("Restore for " << _name << " " << status.ToString() )
  if ( status.ok() )
    return true;
  
  std::vector< ::rocksdb::BackupInfo > info;
  _rdb->GetBackupInfo( &info );
  std::vector< ::rocksdb::BackupID > bads;
  _rdb->GetCorruptedBackups(&bads);
  if ( !bads.empty() )
  {
    std::stringstream ss;
    for (auto b : bads)
      ss << b;
    DOMAIN_LOG_ERROR("Есть поврежденные бэкапы " << ss.str());
  }

  for ( auto inf : info )
  {
    COMMON_LOG_BEGIN("Restore from backup_id=" << inf.backup_id )
    ::rocksdb::Status status = _rdb->RestoreDBFromBackup( inf.backup_id, _conf.path, _conf.path, ::rocksdb::RestoreOptions() );
    COMMON_LOG_END("Restore for " << _name << " " << status.ToString() )
    if ( status.ok() )
      return true;
  }
  return false;
}


}}
