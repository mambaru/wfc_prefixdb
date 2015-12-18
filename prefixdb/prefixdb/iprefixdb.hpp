#pragma once

#include <wfc/iinterface.hpp>
#include <prefixdb/api/set.hpp>
#include <prefixdb/api/get.hpp>
#include <prefixdb/api/has.hpp>
#include <prefixdb/api/del.hpp>
#include <prefixdb/api/inc.hpp>
#include <prefixdb/api/upd.hpp>

namespace wamba { namespace prefixdb {

struct iprefixdb: public ::wfc::iinterface
{
  virtual ~iprefixdb() {}
  virtual void set( request::set::ptr req, response::set::handler cb) = 0;
  virtual void get( request::get::ptr req, response::get::handler cb) = 0;
  virtual void has( request::has::ptr req, response::has::handler cb) = 0;
  virtual void del( request::del::ptr req, response::del::handler cb) = 0;
  virtual void inc( request::inc::ptr req, response::inc::handler cb) = 0;
  virtual void upd( request::upd::ptr req, response::upd::handler cb) = 0;
};

}}
