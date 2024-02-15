#include <prefixdb/domain/multidb/aux/scan_dir.hpp>
#include <dirent.h>
       
namespace wamba{ namespace prefixdb {
  
std::vector<std::string> scan_dir(const std::string& path, bool& fail)
{
  fail = false;
  std::vector<std::string> result;

  DIR *dir;
  dirent *entry;

  dir = opendir(path.c_str());
  if (!dir) 
  {
    fail = true;
    return result;
  };

  while ( (entry = readdir(dir)) != nullptr) 
  {
    if ( entry->d_type == 4)
    {
      std::string file = entry->d_name;
      if ( file=="." || file==".." ) continue;
      result.push_back(file);
    }
  };

  closedir(dir);
  return result;
}
    
}}
