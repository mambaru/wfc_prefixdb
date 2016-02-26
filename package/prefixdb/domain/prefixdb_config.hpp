#pragma once

#include <prefixdb/domain/storage/multidb_config.hpp>
#include <string>
#include <vector>

namespace wamba{ namespace prefixdb{

struct prefixdb_config: multidb_config
{
  // Список объектов wfc, которые сдедуюет остановить перед запуском 
  std::vector< std::string > stop_list;
  bool compact_before_backup = false;
  time_t backup_period_s = 0;
  time_t restore_period_s = 0;
};

}}
