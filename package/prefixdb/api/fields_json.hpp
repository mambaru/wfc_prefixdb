#pragma once

#include <prefixdb/api/set.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

typedef wjson::dict_vector<
  wjson::raw_value<std::string>,
  20
> fields_list_json;

typedef wjson::array< std::vector< wjson::value<std::string> >, 10 > key_list_json;

}}
