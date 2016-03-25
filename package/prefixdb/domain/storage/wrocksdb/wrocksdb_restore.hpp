#pragma once

#include <prefixdb/iprefixdb.hpp>
#include <prefixdb/domain/storage/iprefixdb_ex.hpp>
#include <prefixdb/domain/storage/options/db_config.hpp>
#include <rocksdb/db.h>
#include <rocksdb/utilities/backupable_db.h>

#include <memory>
#include <mutex>

namespace rocksdb{ class RestoreBackupableDB;}

namespace wamba{ namespace prefixdb{
  
class wrocksdb_restore
  : public iprefixdb_restore
{
public:
  typedef ::rocksdb::RestoreBackupableDB restore_db_type;
  wrocksdb_restore( std::string name, const db_config conf, restore_db_type* rdb);
  virtual bool restore() override;
private:
  std::string _name;  
  const db_config _conf;
  std::unique_ptr<restore_db_type> _rdb;
};

}}
