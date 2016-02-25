#pragma once

#include <prefixdb/domain/storage/iprefixdb_ex.hpp>
#include <memory>

namespace wamba{ namespace prefixdb{

struct ifactory
{
  typedef std::shared_ptr<iprefixdb_ex> prefixdb_ptr;
  virtual ~ifactory() {}
  virtual void initialize(std::string db_path, std::string backup_path, std::string restore_path, std::string ini_path) = 0;
  virtual prefixdb_ptr create(std::string prefix, bool create_if_missing) = 0;
};

}}
