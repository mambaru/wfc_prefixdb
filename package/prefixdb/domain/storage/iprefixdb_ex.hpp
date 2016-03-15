#pragma once

#include <prefixdb/iprefixdb.hpp>

namespace wamba { namespace prefixdb {

struct iprefixdb_ex: iprefixdb
{
  virtual ~iprefixdb_ex() {}

  virtual void start( ) = 0;

  virtual void close( ) = 0;
  
  using iprefixdb::backup;
  virtual void backup(bool compact_range) =0;
  virtual void archive(std::string suffix) =0;

  using iprefixdb::restore;
  virtual void restore() = 0;
};

}}
