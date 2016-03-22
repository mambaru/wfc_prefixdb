#pragma once

#include <prefixdb/iprefixdb.hpp>
#include <prefixdb/domain/storage/merge/merge.hpp>
#include <prefixdb/domain/storage/iprefixdb_ex.hpp>
#include <prefixdb/domain/storage/rocksdb_config.hpp>
#include <prefixdb/domain/storage/since_reader.hpp>
#include <rocksdb/db.h>
#include <rocksdb/utilities/backupable_db.h>

#include <memory>
#include <mutex>

namespace rocksdb{ class DB;}

namespace wamba{ namespace prefixdb{
  
 
class rocksdb
  : public iprefixdb_ex
  , public std::enable_shared_from_this<rocksdb>
{
public:
  typedef ::rocksdb::BackupableDB db_type;

  rocksdb( std::string name, const rocksdb_config conf, db_type* db);

  virtual void set( request::set::ptr req, response::set::handler cb) override;
  virtual void get( request::get::ptr req, response::get::handler cb) override;
  virtual void has( request::has::ptr req, response::has::handler cb) override;
  virtual void del( request::del::ptr req, response::del::handler cb) override;
  virtual void inc( request::inc::ptr req, response::inc::handler cb) override;
  virtual void add( request::add::ptr req, response::add::handler cb) override;
  virtual void packed( request::packed::ptr req, response::packed::handler cb) override;
  virtual void range( request::range::ptr req, response::range::handler cb) override;
  virtual void get_updates_since( request::get_updates_since::ptr req, response::get_updates_since::handler cb) override;
  
  virtual void start( ) override;
  virtual void close() override;
  virtual bool backup() override;
  virtual bool archive(std::string path) override;

private:

  template<merge_mode Mode, typename Res, typename ReqPtr, typename Callback>
  void merge_(ReqPtr req, Callback cb);
  
  template<typename Res, typename ReqPtr, typename Callback>
  void get_(ReqPtr req, Callback cb);

  template<typename Res, typename Batch, typename ReqPtr, typename Callback>
  void write_batch_(Batch& batch, ReqPtr req, Callback cb);

  void prebackup_(bool compact_range);
  
  typedef wfc::workflow::callback_timer_handler timer_handler;
  typedef wfc::workflow::timer_id_t timer_id_t;
  typedef std::shared_ptr< request::get_updates_since > request_since_ptr;
  void create_slave_timer_();
  void query_updates_since_(std::weak_ptr<iprefixdb> master, timer_handler handler, request_since_ptr preq);
  void result_handler_updates_since_(std::weak_ptr<iprefixdb> master, timer_handler handler, request_since_ptr preq, response::get_updates_since::ptr res);
private:
  
  std::string _name;  
  const rocksdb_config _conf;
  std::unique_ptr<db_type> _db;
  //std::shared_ptr<iprefixdb> _master;
  std::mutex _backup_mutex;
  timer_id_t _slave_timer_id;
  since_reader _reader;
};

class rocksdb_restore
  : public iprefixdb_restore
{
public:
  typedef ::rocksdb::RestoreBackupableDB restore_db_type;
  rocksdb_restore( std::string name, const rocksdb_config conf, restore_db_type* rdb);
  virtual bool restore() override;
private:
  std::string _name;  
  const rocksdb_config _conf;
  std::unique_ptr<restore_db_type> _rdb;
};

}}
