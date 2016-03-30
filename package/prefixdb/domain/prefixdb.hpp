#pragma once

#include <prefixdb/domain/prefixdb_config.hpp>
//#include <prefixdb/domain/storage/multidb.hpp>
#include <prefixdb/iprefixdb.hpp>

#include <wfc/domain_object.hpp>
#include <wfc/workflow.hpp>
#include <iow/io/timer/timer.hpp>
#include <memory>

namespace wamba{ namespace prefixdb{

class multidb;

class prefixdb
  : public ::wfc::domain_object<iprefixdb, prefixdb_config>
  , public std::enable_shared_from_this< prefixdb >
{
  // class impl;
public:
  // domain_object
  virtual void start(const std::string&) override;
  virtual void configure() override;
  virtual void reconfigure() override;
  virtual void stop(const std::string&) override;
  
  // iprefixdb
  virtual void set( request::set::ptr req, response::set::handler cb) override;
  virtual void get( request::get::ptr req, response::get::handler cb) override;
  virtual void has( request::has::ptr req, response::has::handler cb) override;
  virtual void del( request::del::ptr req, response::del::handler cb) override;
  virtual void inc( request::inc::ptr req, response::inc::handler cb) override;
  virtual void add( request::add::ptr req, response::add::handler cb) override;
  virtual void setnx( request::setnx::ptr req, response::setnx::handler cb) override;
  virtual void packed( request::packed::ptr req, response::packed::handler cb) override;
  virtual void range( request::range::ptr req, response::range::handler cb) override;
  virtual void get_updates_since( request::get_updates_since::ptr req, response::get_updates_since::handler cb) override;
  virtual void get_all_prefixes( request::get_all_prefixes::ptr req, response::get_all_prefixes::handler cb) override;
  virtual void detach_prefixes( request::detach_prefixes::ptr req, response::detach_prefixes::handler cb) override;
private:
  std::shared_ptr<multidb> _impl;
  
};

}}
