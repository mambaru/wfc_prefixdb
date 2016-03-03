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
  rocksdb_factory( ::iow::asio::io_service& io);
  typedef ifactory::prefixdb_ptr prefixdb_ptr;
  virtual void initialize(const rocksdb_config& conf) override;
  virtual ifactory::prefixdb_ptr create(std::string dbname, bool create_if_missing) override;
private:
  struct context;
  ::iow::asio::io_service& _io;
  std::shared_ptr<context> _context;
  mutable std::mutex _mutex;
};

}}
