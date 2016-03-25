#pragma once

#include <string>

namespace wamba{ namespace prefixdb{
  
struct restore_config
{
  // запретить востановление для данного конфига
  bool forbid = false;

  // архив бэкапов
  std::string path  = "./prefixdb_archive";
};


}}
