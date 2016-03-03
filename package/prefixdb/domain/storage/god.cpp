#include "rocksdb_factory.hpp"
#include <wfc/logger.hpp>
#include <wfc/asio.hpp>

namespace wamba{ namespace prefixdb{ namespace god{

std::shared_ptr<ifactory> create(std::string type, ::wfc::asio::io_service& io)
{
  COMMON_LOG_MESSAGE("CREATE FACTORY [" << type << "]")

  if ( type == "rocksdb" )
  {
    return std::make_shared<rocksdb_factory>(io);
  }
  return nullptr;
}

}}}
