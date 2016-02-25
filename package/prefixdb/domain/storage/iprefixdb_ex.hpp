#pragma once

#include <prefixdb/iprefixdb.hpp>

namespace wamba { namespace prefixdb {

struct iprefixdb_ex: iprefixdb
{
  virtual ~iprefixdb_ex() {}
  virtual void close( ) = 0;
  using iprefixdb::backup;
  virtual void backup(bool compact_range) =0;
};

}}
