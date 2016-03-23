#pragma once

#include <wfc/iinterface.hpp>
#include <prefixdb/api/set.hpp>
#include <prefixdb/api/setnx.hpp>
#include <prefixdb/api/get.hpp>
#include <prefixdb/api/has.hpp>
#include <prefixdb/api/del.hpp>
#include <prefixdb/api/inc.hpp>
#include <prefixdb/api/add.hpp>
#include <prefixdb/api/packed.hpp>
#include <prefixdb/api/range.hpp>
#include <prefixdb/api/get_updates_since.hpp>
#include <prefixdb/api/get_all_prefixes.hpp>

namespace wamba { namespace prefixdb {

struct iprefixdb: public ::wfc::iinterface
{
  virtual ~iprefixdb() {}
  virtual void set( request::set::ptr req, response::set::handler cb) = 0;
  virtual void get( request::get::ptr req, response::get::handler cb) = 0;
  virtual void has( request::has::ptr req, response::has::handler cb) = 0;
  virtual void del( request::del::ptr req, response::del::handler cb) = 0;
  virtual void inc( request::inc::ptr req, response::inc::handler cb) = 0;
  virtual void add( request::add::ptr req, response::add::handler cb) = 0;
  virtual void setnx( request::setnx::ptr req, response::setnx::handler cb) = 0;
  virtual void packed( request::packed::ptr req, response::packed::handler cb) = 0;
  virtual void range( request::range::ptr req, response::range::handler cb) = 0;
  virtual void get_updates_since( request::get_updates_since::ptr req, response::get_updates_since::handler cb) = 0;
  virtual void get_all_prefixes( request::get_all_prefixes::ptr req, response::get_all_prefixes::handler cb) = 0;
};

}}
