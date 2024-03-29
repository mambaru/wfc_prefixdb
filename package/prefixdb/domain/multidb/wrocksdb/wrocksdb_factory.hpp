#pragma once

#include <prefixdb/iprefixdb.hpp>
#include <prefixdb/domain/multidb/options/db_config.hpp>
#include <prefixdb/domain/multidb/ifactory.hpp>
#include <wfc/asio.hpp>
#include <memory>
#include <mutex>

namespace wamba{ namespace prefixdb{

class wrocksdb_factory
  : public ifactory
{
public:
  typedef boost::asio::io_context io_context_type;
  virtual ~wrocksdb_factory();
  explicit wrocksdb_factory();
  typedef ifactory::prefixdb_ptr prefixdb_ptr;
  virtual bool initialize(const db_config& db_conf) override;
  virtual ifactory::prefixdb_ptr create_db(std::string dbname, bool create_if_missing) override;
  virtual restore_ptr create_restore(std::string dbname) override;
private:
  struct context;
  std::shared_ptr<context> _context;
  mutable std::mutex _mutex;
  uint32_t _ttl=0;
  std::map<std::string, uint32_t> _ttl_map;
};

}}
