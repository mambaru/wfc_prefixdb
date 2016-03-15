#pragma once

#include <prefixdb/domain/storage/multidb_config.hpp>
#include <wfc/workflow.hpp>
#include <string>
#include <vector>

namespace wamba{ namespace prefixdb{

struct prefixdb_config: multidb_config
{
  // Список объектов wfc, которые сдедуюет остановить перед запуском 
  std::vector< std::string > stop_list;
  
  std::string backup_time  = "04:00:00";
  std::string archive_time = "05:00:00";
  
  time_t backup_period_s = 0;
  time_t archive_period_s = 0;
  bool compact_before_backup = false;
  ::wfc::workflow_options workflow;
};

}}
