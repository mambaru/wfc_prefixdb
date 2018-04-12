#pragma once

#include <string>
#include <ctime>

namespace wamba{ namespace prefixdb{
  
struct backup_config
{
  bool enabled = false;
  
  // Путь к бэкапу базы данных для всех префиксов
  std::string path = "";
  
  std::string start_time  = "03:00:00";
  
  time_t period_s = 600;
  
  // количество сохраненных backup-ов(точек с которых можно восстановиться ) (создаються каждые period_s)
  size_t depth = 10;
  
  // не реализовано
  /*time_t start_delay_s = 0;*/
  
};


}}
