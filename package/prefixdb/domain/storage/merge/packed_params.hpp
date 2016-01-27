#pragma once

#include <string>
#include <vector>

namespace wamba{ namespace prefixdb{


struct packed_field_params
{
  std::string inc = "null";
  std::string val = "null";
};

typedef std::vector< std::pair<std::string, packed_field_params> > packed_params_t;

}}
