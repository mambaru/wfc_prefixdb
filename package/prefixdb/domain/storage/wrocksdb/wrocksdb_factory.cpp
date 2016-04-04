
#include "wrocksdb_factory.hpp"
#include <rocksdb/db.h>
#include <rocksdb/env.h>
#include <rocksdb/options.h>
#include <rocksdb/merge_operator.h>
#include <rocksdb/wal_filter.h>
#include <rocksdb/utilities/backupable_db.h>

#include <prefixdb/logger.hpp>
#include <memory>
#include <iostream>
#include <sys/stat.h>
#include <list>
#include <string>

#include "wrocksdb.hpp"
#include "wrocksdb_restore.hpp"
#include "merge/merge_operator.hpp"
#include "wal_buffer.hpp"

namespace rocksdb
{

Status LoadOptionsFromFile(const std::string& options_file_name, Env* env,
                           DBOptions* db_options,
                           std::vector<ColumnFamilyDescriptor>* cf_descs);

}


namespace wamba{ namespace prefixdb{
  

struct wrocksdb_factory::context
{
  typedef ::rocksdb::ColumnFamilyDescriptor CFD;
  typedef std::vector<CFD> CFD_list;
  ::rocksdb::Env* env;
  ::rocksdb::Options options;
  CFD_list cdf;
  /*
  std::string path;
  std::string backup_path;
  std::string restore_path;
  */
  db_config config;

};

wrocksdb_factory::~wrocksdb_factory()
{
  _context->env = nullptr;
}

wrocksdb_factory::wrocksdb_factory( ::iow::asio::io_service& io)
  : _io(io)
 // , _restore(false)
{
}

void wrocksdb_factory::initialize(const db_config& conf1/*, bool restore*/) 
{
  db_config conf = conf1;
  while ( !conf.path.empty() && conf.path.back()=='/' ) conf.path.pop_back();
  while ( !conf.detach_path.empty() && conf.detach_path.back()=='/' ) conf.path.pop_back();
  while ( !conf.backup.path.empty() && conf.backup.path.back()=='/' ) conf.backup.path.pop_back();
  while ( !conf.restore.path.empty() && conf.restore.path.back()=='/' ) conf.restore.path.pop_back();
  while ( !conf.archive.path.empty() && conf.archive.path.back()=='/' ) conf.archive.path.pop_back();
  
  if ( !conf.path.empty() )
  {
    if ( conf.backup.enabled && conf.backup.path.empty() ) conf.backup.path = conf.path + "_backup";
    if ( !conf.restore.forbid && conf.restore.path.empty() ) conf.restore.path = conf.path + "_restore";
    if ( conf.archive.enabled && conf.archive.path.empty() ) conf.archive.path = conf.path + "_archive";  
  }
    
  std::lock_guard<std::mutex> lk(_mutex);  
  //_restore = restore;
  _context = std::make_shared<wrocksdb_factory::context>();
  _context->env = ::rocksdb::Env::Default();
  _context->config = conf;
  
  auto status = ::rocksdb::LoadOptionsFromFile( conf.ini, _context->env, &(_context->options), &(_context->cdf) );
  if ( !status.ok() )
  {
    DOMAIN_LOG_FATAL("rocksdb_factory::initialize: " << status.ToString());
    abort();
  }

  _context->cdf[0].options.merge_operator = std::make_shared<merge_operator>(conf.array_limit, conf.packed_limit);
  
}

class wall_filter
  : public ::rocksdb::WalFilter
{
  typedef ::rocksdb::WalFilter super;
  typedef std::shared_ptr<wal_buffer> buffer_ptr;
public:
  
  wall_filter(const std::string& name, buffer_ptr buff)
    : _name(name)
    , _buffer(buff)
  {}
    
  virtual super::WalProcessingOption LogRecord(const ::rocksdb::WriteBatch& batch,
                                        ::rocksdb::WriteBatch* /*new_batch*/,
                                        bool* batch_changed) const override
  {
      PREFIXDB_LOG_DEBUG(_name << "----------->WAL batch.data=" << batch.Data() );
      _buffer->add(batch.Data());
      *batch_changed = false;
      return super::WalProcessingOption::kContinueProcessing;
  }

  // Returns a name that identifies this WAL filter.
  // The name will be printed to LOG file on start up for diagnosis.
  virtual const char* Name() const override
  {
    return _name.c_str();
  }
private:
  std::string _name;
  buffer_ptr _buffer;
};

//::rocksdb::RestoreBackupableDB
ifactory::prefixdb_ptr wrocksdb_factory::create_db(std::string dbname, bool create_if_missing) 
{
  std::lock_guard<std::mutex> lk(_mutex);
  _context->options.env = _context->env;
  _context->options.create_if_missing = create_if_missing;
  auto conf = _context->config;
  if ( conf.master.enabled )
  {
    auto buff = std::make_shared<wal_buffer>(dbname, conf.master.log_buffer_size);
    conf.master.walbuf = buff;
   _context->options.wal_filter = new wall_filter(dbname, buff);
  }

  
  if ( !conf.path.empty() ) conf.path = _context->config.path + "/" + dbname;
  if ( !conf.detach_path.empty() ) conf.detach_path = _context->config.detach_path + "/" + dbname;
  if ( !conf.backup.path.empty()  ) conf.backup.path = _context->config.backup.path + "/" + dbname;
  if ( !conf.restore.path.empty() ) conf.restore.path = _context->config.restore.path + "/" + dbname;
  
  ::rocksdb::DB* db;
  std::vector< ::rocksdb::ColumnFamilyHandle*> handles;
  
  auto status = ::rocksdb::DB::Open(_context->options, conf.path, _context->cdf , &handles, &db);
  if ( status.ok() ) {
    assert(handles.size() == 1);
    // i can delete the handle since DBImpl is always holding a reference to
    // default column family
    delete handles[0];
  }

  if ( status.ok() )
  {
    COMMON_LOG_MESSAGE("Backup path: " << conf.backup.path)
    ::rocksdb::BackupableDBOptions backup_opt( conf.backup.path );
    //backup_opt.destroy_old_data = true; //???? 
    auto bdb = new ::rocksdb::BackupableDB(db, backup_opt);
    if ( !conf.restore.path.empty() )
    {
      ::rocksdb::BackupableDBOptions restore_opt( conf.restore.path );
      DEBUG_LOG_MESSAGE("New RocksDB Restore " << restore_opt.backup_dir)
    }
    
    
    DEBUG_LOG_MESSAGE("New RocksDB " << dbname)
    return std::make_shared< wrocksdb >(dbname, conf, bdb);
  }

  DOMAIN_LOG_FATAL("rocksdb_factory::create: " << status.ToString());
  return nullptr;
}


wrocksdb_factory::restore_ptr wrocksdb_factory::create_restore(std::string dbname) 
{
  //_context->options.env = _context->env;
  auto conf = _context->config;
  if ( !conf.path.empty() ) conf.path = _context->config.path + "/" + dbname;
//  if ( !conf.backup.path.empty()  ) conf.backup.path = _context->config.backup.path + "/" + dbname;
  if ( !conf.restore.path.empty() ) conf.restore.path = _context->config.restore.path + "/" + dbname;

  
  ::rocksdb::RestoreBackupableDB* rdb = nullptr;
  if ( !conf.restore.path.empty() )
  {
    ::rocksdb::BackupableDBOptions restore_opt( conf.restore.path );
    DEBUG_LOG_MESSAGE("New RocksDB Restore " << restore_opt.backup_dir)
    rdb = new ::rocksdb::RestoreBackupableDB( _context->env, restore_opt);
    return std::make_shared< wrocksdb_restore >(dbname, conf, rdb);
  }
  return nullptr;
  
}

}}
