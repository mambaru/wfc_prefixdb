#pragma once

#include <prefixdb/api/aux/field_base.hpp>

namespace wamba{ namespace prefixdb{

struct basic_field: field_base
{
  std::string val;
};

}}
