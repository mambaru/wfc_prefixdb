#pragma once

#include <string>
#include <wjson/wjson.hpp>


namespace wamba{ namespace prefixdb{

typedef wjson::dict_vector< wjson::raw_value<std::string>, 10 > packed_json;

}}
