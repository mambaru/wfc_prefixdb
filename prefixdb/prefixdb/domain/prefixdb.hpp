#pragma once

#include <prefixdb/domain/prefixdb_config.hpp>
#include <prefixdb/iprefixdb.hpp>

#include <wfc/domain_object.hpp>

namespace wamba{ namespace prefixdb{

class prefixdb
  : public ::wfc::domain_object<iprefixdb, prefixdb_config>
{
public:
  virtual void reconfigure() override;
  virtual void set( request::set::ptr req, response::set::handler cb) override;
  virtual void get( request::get::ptr req, response::get::handler cb) override;
  virtual void has( request::has::ptr req, response::has::handler cb) override;
};

}}
