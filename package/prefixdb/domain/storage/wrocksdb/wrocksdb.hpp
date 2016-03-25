#pragma once

#include <prefixdb/iprefixdb.hpp>
#include <prefixdb/domain/storage/wrocksdb/merge/merge.hpp>
#include <prefixdb/domain/storage/iprefixdb_ex.hpp>
#include <prefixdb/domain/storage/options/db_config.hpp>
#include <rocksdb/db.h>
#include <rocksdb/utilities/backupable_db.h>

#include <memory>
#include <mutex>

namespace rocksdb{ class BackupableDB;}

namespace wamba{ namespace prefixdb{
  
class wrocksdb_slave;

class wrocksdb
  : public iprefixdb_ex
  , public std::enable_shared_from_this<wrocksdb>
{
public:
  typedef ::rocksdb::BackupableDB db_type;

  wrocksdb( std::string name, const db_config conf, db_type* db);

  virtual void set( request::set::ptr req, response::set::handler cb) override;
  virtual void setnx( request::setnx::ptr req, response::setnx::handler cb) override;
  virtual void get( request::get::ptr req, response::get::handler cb) override;
  virtual void has( request::has::ptr req, response::has::handler cb) override;
  virtual void del( request::del::ptr req, response::del::handler cb) override;
  virtual void inc( request::inc::ptr req, response::inc::handler cb) override;
  virtual void add( request::add::ptr req, response::add::handler cb) override;
  virtual void packed( request::packed::ptr req, response::packed::handler cb) override;
  virtual void range( request::range::ptr req, response::range::handler cb) override;
  
  virtual void get_updates_since( request::get_updates_since::ptr req, response::get_updates_since::handler cb) override;
  virtual void get_all_prefixes( request::get_all_prefixes::ptr req, response::get_all_prefixes::handler cb) override;
  
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

  typedef wfc::workflow::callback_timer_handler timer_handler;
  typedef wfc::workflow::timer_id_t timer_id_t;
  typedef std::shared_ptr< request::get_updates_since > request_since_ptr;

private:
  
  std::string _name;  
  const db_config _conf;
  std::unique_ptr<db_type> _db;
  std::mutex _backup_mutex;
  std::shared_ptr<wrocksdb_slave> _slave;
};


}}
