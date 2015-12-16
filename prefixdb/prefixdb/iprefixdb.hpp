#pragma once

#include <wfc/iinterface.hpp>
#include <prefixdb/api/set.hpp>
#include <prefixdb/api/get.hpp>
#include <prefixdb/api/has.hpp>

namespace wamba { namespace prefixdb {

struct iprefixdb: public ::wfc::iinterface
{
  virtual ~iprefixdb() {}
  virtual void set( request::set::ptr req, response::set::handler cb) = 0;
  virtual void get( request::get::ptr req, response::get::handler cb) = 0;
  virtual void has( request::has::ptr req, response::has::handler cb) = 0;
};

}}
