#pragma once
#include <string>     

namespace wamba{ namespace prefixdb { namespace file{

bool copy(const std::string& from, const std::string& to, std::string& message);

bool move(const std::string& from, const std::string& to, std::string& message);

bool remove(const std::string& path, std::string& message);

bool exist(const std::string& path);

}}}
