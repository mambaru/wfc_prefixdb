#pragma once

#include <prefixdb/api/aux/field_type.hpp>
#include <prefixdb/api/aux/key_field.hpp>

namespace wamba{ namespace prefixdb{

struct field_base: key_field
{
  time_t ttl = 0;
  field_type type = field_type::any; // number, string, package, any, none - запрещено
};

}}
