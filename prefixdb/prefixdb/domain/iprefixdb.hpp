#pragma once

#include <wfc/iinterface.hpp>
#include <prefixdb/api/set.hpp>

namespace wamba { namespace prefixdb {

struct iprefixdb: public ::wfc::iinterface
{
  virtual ~iprefixdb() {}
  virtual void set( request::set::ptr req, response::set::handler cb) = 0;
};

}}
