#pragma once

#include <prefixdb/domain/multidb/wrocksdb/wrocksdb.hpp>
#include <prefixdb/domain/multidb/options/db_config.hpp>
#include <memory>

namespace wamba{ namespace prefixdb{

struct ifactory
{
  typedef std::shared_ptr<wrocksdb> prefixdb_ptr;
  typedef std::shared_ptr<iprefixdb_restore> restore_ptr;
  virtual ~ifactory() {}
  virtual bool initialize(const db_config& conf) = 0;
  virtual prefixdb_ptr create_db(std::string prefix,  bool create_if_missing)  = 0;
  virtual restore_ptr  create_restore(std::string prefix) = 0;
};

}}
