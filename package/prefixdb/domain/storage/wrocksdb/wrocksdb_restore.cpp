
#include "wrocksdb_restore.hpp"
#include <prefixdb/logger.hpp>
#include <rocksdb/db.h>
#include <rocksdb/utilities/backupable_db.h>



namespace wamba{ namespace prefixdb {

wrocksdb_restore::wrocksdb_restore(std::string name, const db_config conf, restore_db_type* rdb)
  : _name(name)
  , _conf(conf)
  , _rdb(rdb)
{
  
}
bool wrocksdb_restore::restore() 
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
