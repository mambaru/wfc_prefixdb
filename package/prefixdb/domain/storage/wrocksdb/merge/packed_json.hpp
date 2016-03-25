#pragma once

#include <string>
#include <wfc/json.hpp>
#include "packed.hpp"

namespace wamba{ namespace prefixdb{

typedef ::wfc::json::object2array<
  ::wfc::json::value<std::string>,
  ::wfc::json::raw_value<std::string>,
  10
> packed_json;

}}
