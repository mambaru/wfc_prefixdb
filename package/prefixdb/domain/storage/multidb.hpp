#pragma once

#include <prefixdb/iprefixdb.hpp>
#include "multidb_options.hpp"
#include <memory>
#include <map>
#include <mutex>


namespace wamba{ namespace prefixdb{
 
struct ifactory;

class multidb
  : public iprefixdb
{
  typedef std::shared_ptr<iprefixdb> prefixdb_ptr;
  typedef std::map<std::string, prefixdb_ptr> db_map;
public:
  void reconfigure(const multidb_options opt);
  void stop();
  virtual void set( request::set::ptr req, response::set::handler cb) override;
  virtual void get( request::get::ptr req, response::get::handler cb) override;
  virtual void has( request::has::ptr req, response::has::handler cb) override;
  virtual void del( request::del::ptr req, response::del::handler cb) override;
  virtual void inc( request::inc::ptr req, response::inc::handler cb) override;
  virtual void upd( request::upd::ptr req, response::upd::handler cb) override;
private:
  prefixdb_ptr prefix_(const std::string& prefix, bool create_if_missing);
  
private:
  std::shared_ptr<ifactory> _factory;
  db_map _db_map;
  std::mutex _mutex;
};

}}
