#pragma once

#include <string>
#include <vector>

namespace wamba{ namespace prefixdb{


struct add_params
{
  size_t lim = 10;
  std::vector<std::string> arr;  
};

}}
