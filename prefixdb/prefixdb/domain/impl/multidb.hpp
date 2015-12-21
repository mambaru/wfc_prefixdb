#pragma once

#include <prefixdb/iprefixdb.hpp>

#include <wfc/domain_object.hpp>
#include <memory>

namespace wamba{ namespace prefixdb{

class multidb
  : public iprefixdb
{
public:
  virtual void set( request::set::ptr req, response::set::handler cb) override;
  virtual void get( request::get::ptr req, response::get::handler cb) override;
  virtual void has( request::has::ptr req, response::has::handler cb) override;
  virtual void del( request::del::ptr req, response::del::handler cb) override;
  virtual void inc( request::inc::ptr req, response::inc::handler cb) override;
  virtual void upd( request::upd::ptr req, response::upd::handler cb) override;
};

}}
