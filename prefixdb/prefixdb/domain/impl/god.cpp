#include "rocksdb_factory.hpp"

namespace wamba{ namespace prefixdb{ namespace god{

std::shared_ptr<ifactory> create(std::string type)
{
  std::shared_ptr<ifactory> result;
  if ( type == "rocksdb" )
  {
    result = std::make_shared<rocksdb_factory>();
  }
  
  return result;
}

}}}
