#pragma once

#include <prefixdb/iprefixdb.hpp>
#include <prefixdb/prefixdb/multidb/wrocksdb/merge/merge.hpp>
#include <prefixdb/prefixdb/multidb/iprefixdb_ex.hpp>
#include <prefixdb/prefixdb/multidb/options/db_config.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <rocksdb/db.h>
#include <rocksdb/write_batch.h>
#include <rocksdb/utilities/backupable_db.h>
#include <rocksdb/utilities/db_ttl.h>
#pragma GCC diagnostic pop

#include <memory>
#include <mutex>

namespace rocksdb{ class BackupEngine;}

namespace wamba{ namespace prefixdb{
  
class wrocksdb_slave;

class wrocksdb
  : public iprefixdb_ex
  , public std::enable_shared_from_this<wrocksdb>
{
public:
  typedef ::rocksdb::BackupEngine backup_type;
  typedef ::rocksdb::DBWithTTL  db_type;
  typedef ::rocksdb::Snapshot snapshot_type;
  typedef const snapshot_type* snapshot_ptr;

  wrocksdb( std::string name, const db_config conf, db_type* db, backup_type* bk);

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
  virtual void detach_prefixes( request::detach_prefixes::ptr req, response::detach_prefixes::handler cb) override;
  virtual void attach_prefixes( request::attach_prefixes::ptr req, response::attach_prefixes::handler cb) override;
  virtual void delay_background( request::delay_background::ptr req, response::delay_background::handler cb) override;
  virtual void continue_background( request::continue_background::ptr req, response::continue_background::handler cb) override;
  virtual void compact_prefix( request::compact_prefix::ptr req, response::compact_prefix::handler cb) override;
  
  virtual void create_snapshot( request::create_snapshot::ptr req, response::create_snapshot::handler cb) override;
  virtual void release_snapshot( request::release_snapshot::ptr req, response::release_snapshot::handler cb) override;
  
  virtual void start( ) override;
  virtual void stop() override;
  virtual bool compact() override;
  virtual bool backup() override;
  virtual bool archive(std::string path) override;

private:

  void stop_();

  bool check_inc_(request::inc::ptr& req, response::inc::handler& cb);
  bool check_add_(request::add::ptr& req, response::add::handler& cb);
  bool check_packed_(request::packed::ptr& req, response::packed::handler& cb);

  template<merge_mode Mode, typename Res, typename ReqPtr, typename Callback>
  void merge_(ReqPtr req, Callback cb);
  
  template<typename Res, typename ReqPtr, typename Callback>
  void get_(ReqPtr req, Callback cb, bool ignore_if_missing = false);

  template<typename Res, typename BatchPtr, typename ReqPtr, typename Callback>
  void write_batch_(BatchPtr batch, ReqPtr req, Callback cb);

  snapshot_ptr find_snapshot_(size_t id) const;
  size_t create_snapshot_();
  bool release_snapshot_(size_t id);
private:
  
  std::string _name;  
  const db_config _conf;
  std::shared_ptr<db_type> _db;
  std::shared_ptr<backup_type> _backup;
  mutable std::mutex _mutex;
  size_t _snapshot_counter = 0;
  std::map<size_t, snapshot_ptr> _snapshot_map;
  std::shared_ptr<wrocksdb_slave> _slave;
  std::shared_ptr< ::wfc::workflow> _flow;
};


}}
