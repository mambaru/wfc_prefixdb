#pragma once

#include <prefixdb/domain/storage/multidb_config.hpp>
#include <prefixdb/domain/storage/iprefixdb_ex.hpp>
#include <memory>
#include <map>
#include <mutex>


namespace wamba{ namespace prefixdb{
 
struct ifactory;

class multidb
  : public iprefixdb_ex
{
  typedef std::shared_ptr<iprefixdb_ex> prefixdb_ptr;
  typedef std::map<std::string, prefixdb_ptr> db_map;
public:
  bool reconfigure(const multidb_config& opt, std::shared_ptr<ifactory> factory);
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

  virtual void start() override;
  virtual void close() override;
  virtual void backup(bool compact_range) override;
  virtual void archive(std::string suffix) override;
  virtual void restore() override;
private:
  bool preopen_(std::string path, bool create_if_missing);
  
  prefixdb_ptr prefix_(const std::string& prefix, bool create_if_missing);
  
  template<typename Res, typename ReqPtr, typename Callback>
  bool check_fields_(const ReqPtr& req, const Callback& cb);

  template<typename Res, typename ReqPtr, typename Callback>
  bool check_prefix_(const ReqPtr& req, const Callback& cb);

  std::vector< std::string > all_prefixes_();
private:
  std::shared_ptr<ifactory> _factory;
  db_map _db_map;
  std::mutex _mutex;
  multidb_config _opt;
};

}}
