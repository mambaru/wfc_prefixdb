#pragma once

#include <string>
#include <ctime>

namespace wamba{ namespace prefixdb{
  
struct backup_config
{
  bool enabled = false;
  
  // Путь к бэкапу базы данных для всех префиксов
  std::string path = "";
  
  /*03:00:00*/
  std::string start_time  = "";
  
  time_t period_s = 0;
  
  // количество сохраненных backup-ов(точек с которых можно восстановиться ) (создаються каждые period_s)
  size_t depth = 1;
  
  // не реализовано
  /*time_t start_delay_s = 0;*/
  
};


}}
