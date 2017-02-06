#pragma once

#include <string>
#include <ctime>

namespace wamba{ namespace prefixdb{

struct compact_config
{
  bool enabled = false;
  bool startup_compact = true;
  std::string start_time  = "02:00:00";
  time_t period_s = 0;
};

}}
