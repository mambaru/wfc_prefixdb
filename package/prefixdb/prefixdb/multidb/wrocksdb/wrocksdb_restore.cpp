#include "wrocksdb_restore.hpp"
#include <prefixdb/logger.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlong-long"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <rocksdb/db.h>
#include <rocksdb/utilities/backupable_db.h>
#pragma GCC diagnostic pop

namespace wamba{ namespace prefixdb {

wrocksdb_restore::wrocksdb_restore(std::string name, const db_config conf, restore_db_type* rdb)
  : _name(name)
  , _conf(conf)
  , _rdb(rdb)
{
  
}
bool wrocksdb_restore::restore() 
{
  PREFIXDB_LOG_BEGIN("Restore for " << _name << " to " << _conf.path << " from " << _conf.restore.path )
  ::rocksdb::Status status = _rdb->RestoreDBFromLatestBackup( _conf.path, _conf.path, ::rocksdb::RestoreOptions() );
  PREFIXDB_LOG_END("Restore for " << _name << " " << status.ToString() )
  if ( status.ok() )
    return true;
  
  std::vector< ::rocksdb::BackupInfo > info;
  _rdb->GetBackupInfo( &info );
  std::vector< ::rocksdb::BackupID > bids;
  _rdb->GetCorruptedBackups(&bids);
  if ( !bids.empty() )
  {
    std::stringstream ss;
    for (auto b : bids)
      ss << b << ",";
    PREFIXDB_LOG_ERROR("Есть поврежденные бэкапы " << ss.str());
  }

  auto bid = _conf.restore.backup_id;
  for ( auto inf : info )
  {
    if ( bid!=0 )
    {
      if ( bid < 0 )
      {
	bid++;
	continue;
      }
      else if ( inf.backup_id < bid )
      {
	continue;
      }
    }
    PREFIXDB_LOG_BEGIN("Restore from backup_id=" << inf.backup_id )
    status = _rdb->RestoreDBFromBackup( inf.backup_id, _conf.path, _conf.path, ::rocksdb::RestoreOptions() );
    PREFIXDB_LOG_END("Restore for " << _name << " " << status.ToString() )
    if ( status.ok() )
      return true;
  }
  return false;
}


}}
