#pragma once


#include <prefixdb/iprefixdb.hpp>
#include <prefixdb/domain/storage/db_config.hpp>
#include <prefixdb/domain/storage/ifactory.hpp>
#include <memory>
#include <mutex>

namespace wamba{ namespace prefixdb{

class wrocksdb_factory
  : public ifactory
{
public:
  virtual ~wrocksdb_factory();
  wrocksdb_factory( ::iow::asio::io_service& io);
  typedef ifactory::prefixdb_ptr prefixdb_ptr;
  virtual void initialize(const db_config& conf, bool restore) override;
  virtual ifactory::prefixdb_ptr create_db(std::string dbname, bool create_if_missing) override;
  virtual restore_ptr create_restore(std::string prefix) override;
private:
  struct context;
  ::iow::asio::io_service& _io;
  std::shared_ptr<context> _context;
  mutable std::mutex _mutex;
  bool _restore;
};

}}
