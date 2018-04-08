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
#include <prefixdb/api/detach_prefixes.hpp>
#include <prefixdb/api/attach_prefixes.hpp>
#include <prefixdb/api/delay_background.hpp>
#include <prefixdb/api/continue_background.hpp>
#include <prefixdb/api/compact_prefix.hpp>

namespace wamba { namespace prefixdb{

struct iprefixdb: public ::wfc::iinterface
{
  virtual ~iprefixdb() {}
  
  virtual void has( request::has::ptr req, response::has::handler cb) = 0;
  virtual void get( request::get::ptr req, response::get::handler cb) = 0;

  virtual void set( request::set::ptr req, response::set::handler cb) = 0;
  virtual void del( request::del::ptr req, response::del::handler cb) = 0;

  virtual void inc( request::inc::ptr req, response::inc::handler cb) = 0;
  virtual void add( request::add::ptr req, response::add::handler cb) = 0;
  virtual void setnx( request::setnx::ptr req, response::setnx::handler cb) = 0;
  virtual void packed( request::packed::ptr req, response::packed::handler cb) = 0;

  virtual void range( request::range::ptr req, response::range::handler cb) = 0;

  virtual void get_all_prefixes( request::get_all_prefixes::ptr req, response::get_all_prefixes::handler cb) = 0;
  virtual void get_updates_since( request::get_updates_since::ptr req, response::get_updates_since::handler cb) = 0;
  
  virtual void detach_prefixes( request::detach_prefixes::ptr req, response::detach_prefixes::handler cb) = 0;
  virtual void attach_prefixes( request::attach_prefixes::ptr req, response::attach_prefixes::handler cb) = 0;
  
  virtual void delay_background( request::delay_background::ptr req, response::delay_background::handler cb) = 0;
  virtual void continue_background( request::continue_background::ptr req, response::continue_background::handler cb) = 0;
  
  virtual void compact_prefix( request::compact_prefix::ptr req, response::compact_prefix::handler cb) = 0;
  
};

}}
