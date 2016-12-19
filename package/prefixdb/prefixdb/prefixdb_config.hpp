#pragma once

#include <prefixdb/prefixdb/storage/options/multidb_config.hpp>
#include <wfc/workflow.hpp>
#include <string>
#include <vector>

namespace wamba{ namespace prefixdb{

struct prefixdb_config: multidb_config
{
  // Список объектов wfc, которые сдедуюет остановить перед запуском 
  std::vector< std::string > stop_list;
};

}}
