#pragma once


#include <prefixdb/iprefixdb.hpp>
#include <prefixdb/domain/storage/rocksdb_config.hpp>
#include <prefixdb/domain/storage/ifactory.hpp>
#include <memory>
#include <mutex>

namespace wamba{ namespace prefixdb{

class rocksdb_factory
  : public ifactory
{
public:
  virtual ~rocksdb_factory();
  typedef ifactory::prefixdb_ptr prefixdb_ptr;
  virtual void initialize(const rocksdb_config& conf) override;
  //virtual void initialize(std::string db_path, std::string backup_path, std::string restore_path, std::string ini_path) override;
  virtual ifactory::prefixdb_ptr create(std::string dbname, bool create_if_missing) override;
private:
  struct context;
  std::shared_ptr<context> _context;
  mutable std::mutex _mutex;
};

}}
