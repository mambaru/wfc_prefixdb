#pragma once


#include <prefixdb/iprefixdb.hpp>
#include <prefixdb/prefixdb/multidb/options/db_config.hpp>
#include <prefixdb/prefixdb/multidb/ifactory.hpp>
#include <wfc/asio.hpp>
#include <memory>
#include <mutex>

namespace wamba{ namespace prefixdb{

class wrocksdb_factory
  : public ifactory
{
public:
  typedef wfc::asio::io_service io_service_type;
  virtual ~wrocksdb_factory();
  wrocksdb_factory( io_service_type& io);
  typedef ifactory::prefixdb_ptr prefixdb_ptr;
  virtual bool initialize(const db_config& conf/*, bool restore*/) override;
  virtual ifactory::prefixdb_ptr create_db(std::string dbname, bool create_if_missing) override;
  virtual restore_ptr create_restore(std::string prefix) override;
private:
  struct context;
  io_service_type& _io;
  std::shared_ptr<context> _context;
  mutable std::mutex _mutex;
  bool _restore;
};

}}
