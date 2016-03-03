
#include "rocksdb_factory.hpp"
#include <rocksdb/db.h>
#include <rocksdb/env.h>
#include <rocksdb/options.h>
#include <rocksdb/merge_operator.h>
#include <rocksdb/utilities/backupable_db.h>

#include <memory>
#include <iostream>
#include <sys/stat.h>
#include <wfc/logger.hpp>
#include <list>

#include "rocksdb.hpp"
#include "merge/merge_operator.hpp"

namespace rocksdb
{

Status LoadOptionsFromFile(const std::string& options_file_name, Env* env,
                           DBOptions* db_options,
                           std::vector<ColumnFamilyDescriptor>* cf_descs);

}


namespace wamba{ namespace prefixdb{
  

struct rocksdb_factory::context
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
  rocksdb_config config;

};

rocksdb_factory::~rocksdb_factory()
{
  _context->env = nullptr;
}

rocksdb_factory::rocksdb_factory( ::iow::asio::io_service& io)
  : _io(io)
{
}

void rocksdb_factory::initialize(const rocksdb_config& conf1) 
{
  
  
  rocksdb_config conf = conf1;
  while ( !conf.path.empty() && conf.path.back()=='/' ) conf.path.pop_back();
  while ( !conf.backup_path.empty() && conf.backup_path.back()=='/' ) conf.backup_path.pop_back();
  while ( !conf.restore_path.empty() && conf.restore_path.back()=='/' ) conf.restore_path.pop_back();
  
  if ( !conf.path.empty() )
  {
    if ( conf.backup_path.empty() ) conf.backup_path = conf.path + "_backup";
    if ( conf.restore_path.empty() ) conf.restore_path = conf.path + "_restore";  
  }
    
  std::lock_guard<std::mutex> lk(_mutex);  
  _context = std::make_shared<rocksdb_factory::context>();
  _context->env = ::rocksdb::Env::Default();
  _context->config = conf;
  
  auto status = ::rocksdb::LoadOptionsFromFile( conf.ini, _context->env, &(_context->options), &(_context->cdf) );
  if ( !status.ok() )
  {
    DOMAIN_LOG_FATAL("rocksdb_factory::initialize: " << status.ToString());
    abort();
  }

  _context->cdf[0].options.merge_operator = std::make_shared<merge_operator>();
  
}
/*
void rocksdb_factory::initialize(std::string db_path, std::string backup_path, std::string restore_path, std::string ini_path) 
{
  std::lock_guard<std::mutex> lk(_mutex);
#error убрать
  _context = std::make_shared<rocksdb_factory::context>();
  _context->env = ::rocksdb::Env::Default();
  _context->path = db_path;
  _context->backup_path = backup_path;
  _context->restore_path = restore_path;
  //_context->options.merge_operator = std::make_shared<merge_operator>();
  auto status = ::rocksdb::LoadOptionsFromFile(ini_path, _context->env, &(_context->options), &(_context->cdf) );
  if ( !status.ok() )
  {
    DOMAIN_LOG_FATAL("rocksdb_factory::initialize: " << status.ToString());
    abort();
  }

  _context->cdf[0].options.merge_operator = std::make_shared<merge_operator>();
}
*/

//::rocksdb::RestoreBackupableDB
ifactory::prefixdb_ptr rocksdb_factory::create(std::string dbname, bool create_if_missing) 
{
  std::lock_guard<std::mutex> lk(_mutex);
  _context->options.env = _context->env;
  _context->options.create_if_missing = create_if_missing;
  //_context->cdf[0].options.create_if_missing = create_if_missing;
  auto conf = _context->config;
  conf.path = _context->config.path + "/" + dbname;
  conf.backup_path = _context->config.backup_path + "/" + dbname;
  conf.restore_path = _context->config.restore_path + "/" + dbname;

  //std::cout << "----===========#################33333333----->" << conf.path << std::endl;
  
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
    COMMON_LOG_MESSAGE("Backup path: " << conf.backup_path)
    ::rocksdb::BackupableDBOptions backup_opt( conf.backup_path );
    backup_opt.destroy_old_data = true;
    auto bdb = new ::rocksdb::BackupableDB(db, backup_opt);
    ::rocksdb::RestoreBackupableDB* rdb = nullptr;
    if ( !conf.restore_path.empty() )
    {
      ::rocksdb::BackupableDBOptions restore_opt( conf.restore_path );
      rdb = new ::rocksdb::RestoreBackupableDB( ::rocksdb::Env::Default(), restore_opt);
    }
    
    conf.slave.timer = std::make_shared< ::iow::io::timer >(_io);
    DEBUG_LOG_MESSAGE("New RocksDB " << dbname)
    return std::make_shared< rocksdb >(dbname, conf, bdb, rdb);
  }

  DOMAIN_LOG_FATAL("rocksdb_factory::create: " << status.ToString());
  return nullptr;
}

}}
