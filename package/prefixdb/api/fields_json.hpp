#pragma once

#include <prefixdb/api/set.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

typedef ::wfc::json::dict_vector<
  ::wfc::json::raw_value<std::string>,
  20
> fields_list_json;

typedef ::wfc::json::array< std::vector< ::wfc::json::value<std::string> >, 10 > key_list_json;

}}
