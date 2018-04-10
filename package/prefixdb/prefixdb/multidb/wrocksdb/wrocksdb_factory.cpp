
#include "wrocksdb_factory.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <rocksdb/db.h>
#include <rocksdb/env.h>
#include <rocksdb/options.h>
#include <rocksdb/merge_operator.h>
#include <rocksdb/compaction_filter.h>
#include <rocksdb/utilities/backupable_db.h>
#include <rocksdb/utilities/options_util.h>
#include <rocksdb/utilities/db_ttl.h>
#pragma GCC diagnostic pop


#include <prefixdb/logger.hpp>
#include <memory>
#include <iostream>
#include <sys/stat.h>
#include <list>
#include <string>

#include "wrocksdb.hpp"
#include "wrocksdb_restore.hpp"
#include "merge/merge_operator.hpp"


#include <wfc/wfc_exit.hpp>

namespace wamba{ namespace prefixdb{

struct wrocksdb_factory::context
{
  typedef ::rocksdb::ColumnFamilyDescriptor CFD;
  typedef std::vector<CFD> CFD_list;
  ::rocksdb::Env* env;
  ::rocksdb::Options options;
  CFD_list cdf;
  db_config config;
};

wrocksdb_factory::~wrocksdb_factory()
{
  _context->env = nullptr;
}

wrocksdb_factory::wrocksdb_factory( ::iow::asio::io_service& io)
  : _io(io)
{
}

bool wrocksdb_factory::initialize(const db_config& db_conf) 
{
  db_config conf = db_conf;
  while ( !conf.path.empty() && conf.path.back()=='/' ) conf.path.pop_back();
  while ( !conf.wal_path.empty() && conf.wal_path.back()=='/' ) conf.wal_path.pop_back();
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
  _ttl = conf.TTL_seconds;
  _context = std::make_shared<wrocksdb_factory::context>();
  _context->env = ::rocksdb::Env::Default();
  _context->config = conf;
  
  if ( !conf.ini.empty() )
  {
    auto status = ::rocksdb::LoadOptionsFromFile( conf.ini, _context->env, &(_context->options), &(_context->cdf) );
    if ( !status.ok() )
    {
      DOMAIN_LOG_FATAL("rocksdb_factory::initialize: " << status.ToString());
      return false;
    }
  }
  else
  {
    _context->cdf.push_back( context::CFD() );
    _context->options = ::rocksdb::Options();
  }
  return true;
}

ifactory::prefixdb_ptr wrocksdb_factory::create_db(std::string dbname, bool create_if_missing) 
{
  if ( dbname.empty() )
  {
     PREFIXDB_LOG_FATAL("wrocksdb_factory::create_db empty name ")
     return nullptr;
  }
  else
  {
    PREFIXDB_LOG_MESSAGE("wrocksdb_factory::create_db '" << dbname << "'")
  }
  
  std::lock_guard<std::mutex> lk(_mutex);
  _context->options.env = _context->env;
  _context->options.create_if_missing = create_if_missing;
  auto conf = _context->config;
  
  auto merge = std::make_shared<merge_operator>(conf.array_limit, conf.packed_limit);
  _context->cdf[0].options.merge_operator = merge;
  
  if ( !conf.path.empty() ) conf.path = _context->config.path + "/" + dbname;
  if ( !conf.wal_path.empty() ) conf.wal_path = _context->config.wal_path + "/" + dbname;
  if ( !conf.detach_path.empty() ) conf.detach_path = _context->config.detach_path + "/" + dbname;
  //else conf.detach_path = _context->config.detach_path + "/" + dbname + "_detach";
  if ( !conf.backup.path.empty()  ) conf.backup.path = _context->config.backup.path + "/" + dbname;
  if ( !conf.restore.path.empty() ) conf.restore.path = _context->config.restore.path + "/" + dbname;

  auto options = _context->options;
  if ( !conf.wal_path.empty() )
  {
    auto wal_dir = options.wal_dir;
    options.wal_dir = conf.wal_path;
    if ( !wal_dir.empty() && wal_dir!="\"\"" )
      options.wal_dir += std::string("/") + wal_dir;
  }
  
  ::rocksdb::DBWithTTL* db;
  std::vector< ::rocksdb::ColumnFamilyHandle*> handles;
  
  PREFIXDB_LOG_BEGIN("rocksdb::DBWithTTL::Open '" << dbname << "' TTL=" << _ttl << " ...");

  std::vector<int32_t> ttls;
  ttls.push_back(_ttl);
  auto status = ::rocksdb::DBWithTTL::Open(options, conf.path, _context->cdf , &handles, &db, ttls);
  
  if ( !status.ok() )
  {
    PREFIXDB_LOG_ERROR("rocksdb::DB::Open '" << dbname << "' :" << status.ToString());
    if ( conf.auto_repair )
    {
      status = ::rocksdb::RepairDB(conf.path, options );
      PREFIXDB_LOG_MESSAGE("rocksdb::DB::RepairDB '" << dbname << "' :" << status.ToString());
      if ( status.ok() )
      {
        status = ::rocksdb::DBWithTTL::Open(options, conf.path, _context->cdf , &handles, &db, ttls);
      }
    }
  }
  
  if ( !status.ok() )
  {
    if ( conf.abort_if_open_error )
    {
      DOMAIN_LOG_FATAL("Can not open DB " << conf.path);
    }
    return nullptr;
  }
  
  if ( status.ok() ) 
  {
    assert(handles.size() == 1);
    // i can delete the handle since DBImpl is always holding a reference to
    // default column family
    delete handles[0];
  }

  PREFIXDB_LOG_END("rocksdb::DB::Open '" << dbname << "' " )

  if ( status.ok() )
  {
    COMMON_LOG_MESSAGE("Backup path: " << conf.backup.path)
    ::rocksdb::BackupableDBOptions backup_opt( conf.backup.path );
    ::rocksdb::BackupEngine* backup_engine;
    status = ::rocksdb::BackupEngine::Open( 
      _context->env, 
      backup_opt, 
      &backup_engine
    );

    //auto bdb = new ::rocksdb::BackupEngine(db, backup_opt);
    if ( !conf.restore.path.empty() )
    {
      ::rocksdb::BackupableDBOptions restore_opt( conf.restore.path );
    }
    auto pwrdb = std::make_shared< wrocksdb >(dbname, conf, db, backup_engine);
    return pwrdb;
  }

  DOMAIN_LOG_FATAL("rocksdb_factory::create: " << status.ToString());
  return nullptr;
}


wrocksdb_factory::restore_ptr wrocksdb_factory::create_restore(std::string dbname) 
{
  auto conf = _context->config;
  if ( !conf.path.empty() ) conf.path = _context->config.path + "/" + dbname;
  if ( !conf.restore.path.empty() ) conf.restore.path = _context->config.restore.path + "/" + dbname;

  rocksdb::BackupEngineReadOnly* backup_engine = nullptr;
  if ( !conf.restore.path.empty() )
  {
    rocksdb::BackupableDBOptions restore_opt( conf.restore.path );
    //rdb = new ::rocksdb::RestoreBackupableDB( _context->env, restore_opt);
    rocksdb::Status s = rocksdb::BackupEngineReadOnly::Open( _context->env, restore_opt, &backup_engine);
    if ( s.ok() )
      return std::make_shared< wrocksdb_restore >(dbname, conf, backup_engine);
  }
  return nullptr;
  
}

}}
