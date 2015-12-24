#pragma once

#include <string>

namespace wamba{ namespace prefixdb{
  
struct multidb_options
{
  // Путь к базе данных
  std::string path = "/ssd/rocksdb";
  // Файл опций в формате ini
  std::string ini = "./rocksdb.ini";
  // Файл опций в формате ini
  std::string type = "rocksdb";
};

}}
