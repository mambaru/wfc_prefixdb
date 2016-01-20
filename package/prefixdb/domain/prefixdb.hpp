#pragma once

#include <prefixdb/domain/prefixdb_config.hpp>
#include <prefixdb/iprefixdb.hpp>

#include <wfc/domain_object.hpp>
#include <memory>

namespace wamba{ namespace prefixdb{

class prefixdb
  : public ::wfc::domain_object<iprefixdb, prefixdb_config>
{
  class impl;
public:
  virtual void reconfigure() override;
  virtual void set( request::set::ptr req, response::set::handler cb) override;
  virtual void get( request::get::ptr req, response::get::handler cb) override;
  virtual void has( request::has::ptr req, response::has::handler cb) override;
  virtual void del( request::del::ptr req, response::del::handler cb) override;
  virtual void inc( request::inc::ptr req, response::inc::handler cb) override;
  virtual void add( request::add::ptr req, response::add::handler cb) override;
  virtual void packed( request::packed::ptr req, response::packed::handler cb) override;
  virtual void range( request::range::ptr req, response::range::handler cb) override;
private:
  std::shared_ptr<impl> _impl;
};

}}
