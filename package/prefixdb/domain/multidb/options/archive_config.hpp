#pragma once

#include <string>
#include <ctime>

namespace wamba{ namespace prefixdb{
  
struct archive_config
{
  bool enabled = false;

  // архив бэкапов
  std::string path  = "";

  std::string start_time = "05:00:00";
  
  time_t period_s = 0;

  size_t depth = 1;
};

}}
