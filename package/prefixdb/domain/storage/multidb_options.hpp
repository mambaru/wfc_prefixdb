#pragma once

#include <string>

namespace wamba{ namespace prefixdb{
  
struct multidb_options
{
  // Предварительное открытие всех баз префиксов
  bool preopen = true;
  // Путь к базе данных
  std::string path = "./rocksdb";
  // Файл опций в формате ini
  std::string ini = "./rocksdb.ini";
  // Файл опций в формате ini
  std::string type = "rocksdb";
};

}}
