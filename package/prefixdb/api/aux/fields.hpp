#pragma once

#include <string>
#include <vector>
#include <utility>

namespace wamba { namespace prefixdb {

typedef std::pair<std::string, std::string> field_pair;
typedef std::vector< field_pair > raw_field_list_t;
typedef std::vector< std::string > key_list_t;

}}
