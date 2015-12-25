#pragma once

#include <prefixdb/iprefixdb.hpp>
#include "ifactory.hpp"
#include <memory>
#include <mutex>

namespace wamba{ namespace prefixdb{

class rocksdb1_factory
  : public ifactory
{
public:
  virtual ~rocksdb1_factory();
  typedef ifactory::prefixdb_ptr prefixdb_ptr;
  virtual void initialize(std::string db_path, std::string ini_path) override;
  virtual ifactory::prefixdb_ptr create(std::string prefix, bool) override;
private:
  struct context;
  std::shared_ptr<context> _context;
  std::mutex _mutex;
};

}}
