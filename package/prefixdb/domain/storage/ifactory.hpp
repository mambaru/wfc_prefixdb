#pragma once

#include <prefixdb/domain/storage/iprefixdb_ex.hpp>
#include <prefixdb/domain/storage/rocksdb_config.hpp>
#include <memory>

namespace wamba{ namespace prefixdb{

struct ifactory
{
  typedef std::shared_ptr<iprefixdb_ex> prefixdb_ptr;
  typedef std::shared_ptr<iprefixdb_restore> restore_ptr;
  virtual ~ifactory() {}
  virtual void initialize(const rocksdb_config& conf, bool restore) = 0;
  virtual prefixdb_ptr create(std::string prefix, bool create_if_missing)  = 0;
  virtual restore_ptr restore(std::string prefix) = 0;
};

}}
