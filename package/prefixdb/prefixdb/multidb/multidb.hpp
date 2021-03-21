#pragma once

#include <prefixdb/prefixdb/multidb/options/multidb_config.hpp>
#include <prefixdb/prefixdb/multidb/wrocksdb/wrocksdb.hpp>
#include <memory>
#include <map>
#include <mutex>

namespace wamba{ namespace prefixdb{

struct ifactory;

class multidb
  : public iprefixdb
  , public std::enable_shared_from_this<multidb>
{
  typedef std::shared_ptr<wrocksdb> prefixdb_ptr;
  typedef std::map<std::string, prefixdb_ptr> db_map;
public:
  multidb();
  bool configure(const multidb_config& opt, const std::shared_ptr<ifactory>& factory);
  void reconfigure(const multidb_config& opt);
  virtual void start();

  virtual void set( request::set::ptr req, response::set::handler cb) override;
  virtual void setnx( request::setnx::ptr req, response::setnx::handler cb) override;
  virtual void get( request::get::ptr req, response::get::handler cb) override;
  virtual void has( request::has::ptr req, response::has::handler cb) override;
  virtual void del( request::del::ptr req, response::del::handler cb) override;
  virtual void inc( request::inc::ptr req, response::inc::handler cb) override;
  virtual void add( request::add::ptr req, response::add::handler cb) override;
  virtual void packed( request::packed::ptr req, response::packed::handler cb) override;
  virtual void range( request::range::ptr req, response::range::handler cb) override;
  virtual void repair_json( request::repair_json::ptr req, response::repair_json::handler cb) override;

  virtual void get_updates_since( request::get_updates_since::ptr req, response::get_updates_since::handler cb) override;
  virtual void get_all_prefixes( request::get_all_prefixes::ptr req, response::get_all_prefixes::handler cb) override;
  virtual void detach_prefixes( request::detach_prefixes::ptr req, response::detach_prefixes::handler cb) override;
  virtual void attach_prefixes( request::attach_prefixes::ptr req, response::attach_prefixes::handler cb) override;
  virtual void delay_background( request::delay_background::ptr req, response::delay_background::handler cb) override;
  virtual void continue_background( request::continue_background::ptr req, response::continue_background::handler cb) override;
  virtual void compact_prefix( request::compact_prefix::ptr req, response::compact_prefix::handler cb) override;
  virtual void create_snapshot( request::create_snapshot::ptr req, response::create_snapshot::handler cb) override;
  virtual void release_snapshot( request::release_snapshot::ptr req, response::release_snapshot::handler cb) override;

  virtual void stop();
  virtual bool backup();
  virtual bool archive();
  virtual bool restore();
  virtual bool compact();

private:

  void configure_compact_timer_();
  void configure_backup_timer_();
  void configure_archive_timer_();
  void configure_prefix_reqester_();
  request::get_all_prefixes::ptr get_all_prefixes_generator_(response::get_all_prefixes::ptr res);

  std::vector< std::string > all_prefixes_();
  bool preopen_(const std::string& path, bool create_if_missing);

  prefixdb_ptr prefix_(const std::string& prefix, bool create_if_missing);
  bool close_prefix_(const std::string& prefix);

  bool allowed_for_slave_(const std::string& prefix) const;

  template<typename Res, typename ReqPtr, typename Callback>
  bool check_fields_(const ReqPtr& req, const Callback& cb, bool is_writable) const;

  template<typename Res, typename ReqPtr, typename Callback>
  bool check_prefix_(const ReqPtr& req, const Callback& cb, bool is_writable) const;

  template<typename Res, typename ReqPtr, typename Callback>
  bool is_writable_(const ReqPtr& req, const Callback& cb) const;

  bool is_writable_(const std::string& prefix) const;
private:

  typedef wflow::workflow::timer_id_t timer_id_t;
  std::shared_ptr<ifactory> _factory;
  db_map _db_map;
  std::mutex _mutex;
  multidb_config _opt;
  iow::owner _owner;
  std::shared_ptr<wflow::workflow> _workflow;

  wflow::workflow::timer_id_t _compact_timer  = -1;
  wflow::workflow::timer_id_t _backup_timer  = -1;
  wflow::workflow::timer_id_t _archive_timer  = -1;
  wflow::workflow::timer_id_t _prefix_reqester = -1;

  // реконфигурируемые опции
  std::atomic_size_t _range_limit;
  std::atomic_size_t _max_prefixes;
  std::atomic_size_t _prefix_size_limit;
  std::atomic_size_t _keys_per_req;
  std::atomic_size_t _value_size_limit;
  std::atomic_size_t _key_size_limit;
  
  std::vector<std::string> _writable_prefixes;
  std::vector<std::string> _readonly_prefixes;
  
  bool _slave_writable_only = false;
  std::vector<std::string> _slave_allowed_prefixes;
  std::vector<std::string> _slave_denied_prefixes;

};

}}
