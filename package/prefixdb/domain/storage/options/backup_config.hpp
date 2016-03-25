#pragma once

#include <string>
#include <ctime>

namespace wamba{ namespace prefixdb{
  
struct backup_config
{
  bool enabled = false;
  
  // Путь к бэкапу базы данных для всех префиксов
  std::string path   = "./prefixdb_backup";
  
  std::string start_time  = "04:00:00";
  
  time_t period_s = 0;
  
  // количество сохраненных backup-ов(точек с которых можно восстановиться ) (создаються каждые period_s)
  size_t depth = 5;
  
  // не реализовано
  time_t start_delay_s = 0;
  
};


}}
