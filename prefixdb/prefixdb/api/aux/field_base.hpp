#pragma once

#include <prefixdb/api/aux/field_type.hpp>

namespace wamba{ namespace prefixdb{

struct field_base
{
  std::string key;
  time_t ttl = 0;
  field_type type = field_type::any; // number, string, package, any, none - запрещено
};

}}
