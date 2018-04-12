#pragma once

#include <prefixdb/iprefixdb.hpp>

namespace wamba { namespace prefixdb {

struct iprefixdb_ex: iprefixdb
{
  virtual ~iprefixdb_ex() {}
  virtual void start()   = 0;
  virtual void stop()   = 0;
  virtual bool backup()  = 0;
  virtual bool compact()  = 0;
  virtual bool archive(std::string path) = 0;
  
};

struct iprefixdb_restore
{
  virtual ~iprefixdb_restore() {}
  virtual bool restore() = 0;
};

}}
