#pragma once
#include <vector>
#include <string>

namespace wamba{ namespace prefixdb {
  
std::vector<std::string> scan_dir(const std::string& path, bool& fail);
    
}}
