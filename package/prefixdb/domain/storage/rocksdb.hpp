#pragma once

#include <prefixdb/iprefixdb.hpp>
#include <prefixdb/domain/storage/merge/merge.hpp>
#include <prefixdb/domain/storage/iprefixdb_ex.hpp>
#include <prefixdb/domain/storage/rocksdb_config.hpp>
#include <rocksdb/db.h>
#include <rocksdb/utilities/backupable_db.h>

#include <memory>

namespace rocksdb{ class DB;}

namespace wamba{ namespace prefixdb{
 
class rocksdb
  : public iprefixdb_ex
  , public std::enable_shared_from_this<rocksdb>
{
public:
  typedef ::rocksdb::BackupableDB db_type;
  typedef ::rocksdb::RestoreBackupableDB restore_db_type;

  rocksdb( std::string name, const rocksdb_config conf, db_type* db, restore_db_type* rdb);
  //void set_master(std::shared_ptr<iprefixdb> master);
  
  virtual void set( request::set::ptr req, response::set::handler cb) override;
  virtual void get( request::get::ptr req, response::get::handler cb) override;
  virtual void has( request::has::ptr req, response::has::handler cb) override;
  virtual void del( request::del::ptr req, response::del::handler cb) override;
  virtual void inc( request::inc::ptr req, response::inc::handler cb) override;
  virtual void add( request::add::ptr req, response::add::handler cb) override;
  virtual void packed( request::packed::ptr req, response::packed::handler cb) override;
  virtual void range( request::range::ptr req, response::range::handler cb) override;
  virtual void backup( request::backup::ptr req, response::backup::handler cb) override;
  virtual void restore( request::restore::ptr req, response::restore::handler cb) override;
  virtual void get_updates_since( request::get_updates_since::ptr req, response::get_updates_since::handler cb) override;
  
  virtual void start( ) override;
  virtual void close() override;
  virtual void backup(bool compact_range) override;
  virtual void restore() override;

private:

  template<merge_mode Mode, typename Res, typename ReqPtr, typename Callback>
  void merge_(ReqPtr req, Callback cb);
  
  template<typename Res, typename ReqPtr, typename Callback>
  void get_(ReqPtr req, Callback cb);

  template<typename Res, typename Batch, typename ReqPtr, typename Callback>
  void write_batch_(Batch& batch, ReqPtr req, Callback cb);

  void prebackup_(bool compact_range);
  
private:
  std::string _name;  
  const rocksdb_config _conf;
  std::unique_ptr<db_type> _db;
  std::unique_ptr<restore_db_type> _rdb;
  std::shared_ptr<iprefixdb> _master;
  
};

}}
