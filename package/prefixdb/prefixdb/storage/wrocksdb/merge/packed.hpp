#pragma once

#include <string>
#include <vector>
#include <utility>

namespace wamba{ namespace prefixdb{

typedef std::pair<std::string, std::string> packed_field;
typedef std::vector< packed_field > packed_t;

}}
