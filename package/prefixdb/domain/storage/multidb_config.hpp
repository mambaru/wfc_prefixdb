#pragma once

#include <string>
#include "merge/merge_config.hpp"
namespace wamba{ namespace prefixdb{
  
struct multidb_config: merge_config
{
  // Предварительное открытие всех баз префиксов
  bool preopen = true;
  // Путь к базе данных
  std::string path = "./rocksdb";
  // Файл опций в формате ini
  std::string ini = "./rocksdb.ini";
};

}}
