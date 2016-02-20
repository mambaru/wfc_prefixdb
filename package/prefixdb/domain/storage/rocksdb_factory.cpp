
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
  std::string path;
  std::string backup_path;
  std::string restore_path;
};

rocksdb_factory::~rocksdb_factory()
{
  _context->env = nullptr;
}

void rocksdb_factory::initialize(std::string db_path, std::string backup_path, std::string restore_path, std::string ini_path) 
{
  std::lock_guard<std::mutex> lk(_mutex);
  
  _context = std::make_shared<rocksdb_factory::context>();
  _context->env = ::rocksdb::Env::Default();
  _context->path = db_path;
  _context->backup_path = backup_path;
  _context->restore_path = restore_path;
  _context->options.merge_operator = std::make_shared<merge_operator>();
  
  auto status = ::rocksdb::LoadOptionsFromFile(ini_path, _context->env, &(_context->options), &(_context->cdf) );
  if ( !status.ok() )
  {
    DOMAIN_LOG_FATAL("rocksdb_factory::initialize: " << status.ToString());
    abort();
  }
}
//::rocksdb::RestoreBackupableDB
ifactory::prefixdb_ptr rocksdb_factory::create(std::string dbname, bool create_if_missing) 
{
  std::lock_guard<std::mutex> lk(_mutex);
  _context->options.env = _context->env;
  _context->options.create_if_missing = create_if_missing;
  
  std::string path = _context->path + "/" + dbname;
  
  std::string bpath = _context->backup_path;
  if ( bpath.empty() ) bpath = path + "/backup";
  else bpath += std::string("/") + dbname;
  
  std::string rpath = _context->restore_path;
  if ( rpath.empty() ) rpath = path + "/backup";
  else rpath += std::string("/") + dbname;

  ::rocksdb::DB* db;
  
  auto status =  ::rocksdb::DB::Open( _context->options, path, &db);
  if ( status.ok() )
  {
    COMMON_LOG_MESSAGE("Backup path: " << bpath)
    ::rocksdb::BackupableDBOptions backup_opt( bpath );
    backup_opt.destroy_old_data = true;
    auto bdb = new ::rocksdb::BackupableDB(db, backup_opt);
    ::rocksdb::RestoreBackupableDB* rdb = nullptr;
    if ( !rpath.empty() )
    {
      ::rocksdb::BackupableDBOptions restore_opt( rpath );
      rdb = new ::rocksdb::RestoreBackupableDB( ::rocksdb::Env::Default(), restore_opt);
    }
    return std::make_shared< rocksdb >(bdb, rdb);
  }

  DOMAIN_LOG_FATAL("rocksdb_factory::create: " << status.ToString());
  return nullptr;
}

}}
