#include "rocksdb_factory.hpp"
#include <wfc/logger.hpp>

namespace wamba{ namespace prefixdb{ namespace god{

std::shared_ptr<ifactory> create(std::string type)
{
  COMMON_LOG_MESSAGE("CREATE FACTORY [" << type << "]")

  if ( type == "rocksdb" )
  {
    return std::make_shared<rocksdb_factory>();
  }
  return nullptr;
}

}}}
