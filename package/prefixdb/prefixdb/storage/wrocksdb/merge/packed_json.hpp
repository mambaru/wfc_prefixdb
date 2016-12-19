#pragma once

#include <string>
#include <wfc/json.hpp>
#include "packed.hpp"

namespace wamba{ namespace prefixdb{

typedef ::wfc::json::dict_vector<
  ::wfc::json::raw_value<std::string>,
  10
> packed_json;

}}
