#include "god.hpp"
#include "wrocksdb/wrocksdb_factory.hpp"
#include <prefixdb/logger.hpp>
#include <wfc/asio.hpp>

namespace wamba{ namespace prefixdb{

std::shared_ptr<ifactory> god::create(std::string type, wfc::asio::io_service& )
{
  PREFIXDB_LOG_MESSAGE("CREATE FACTORY [" << type << "]")

  if ( type == "rocksdb" )
  {
    return std::make_shared<wrocksdb_factory>();
  }
  return nullptr;
}

}}
