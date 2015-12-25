#include "rocksdb_factory.hpp"
#include "rocksdb1_factory.hpp"
#include <wfc/logger.hpp>

namespace wamba{ namespace prefixdb{ namespace god{

std::shared_ptr<ifactory> create(std::string type)
{
  COMMON_LOG_MESSAGE("CREATE FACTORY [" << type << "]")
  std::shared_ptr<ifactory> result;
  if ( type == "rocksdb" )
  {
    result = std::make_shared<rocksdb_factory>();
  }
  else if ( type == "rocksdb1" )
  {
    result = std::make_shared<rocksdb1_factory>();
  }

  return result;
}

}}}
