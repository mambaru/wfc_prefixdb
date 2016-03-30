#pragma once
#include <string>     

namespace wamba{ namespace prefixdb {

bool copy_dir(const std::string& from, const std::string& to, std::string& message);

bool move_dir(const std::string& from, const std::string& to, std::string& message);

}}
