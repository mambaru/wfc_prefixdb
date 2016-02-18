#pragma once

#include <string>
#include "merge/merge_config.hpp"
namespace wamba{ namespace prefixdb{
  
struct multidb_config: merge_config
{
  // максимальное количество ключей на запрос
  size_t keys_per_req = 100;
  // ограничкение на размер ключа
  size_t key_size_limit = 128;
  // ограничкение на размер значений
  size_t value_size_limit = 1024*10;
  // ограничкение на размер префикса
  size_t prefix_size_limit = 256;
  // ограничкение на общее число префиксов
  size_t max_prefixes = 128;

  
  // Предварительное открытие всех баз префиксов
  bool preopen = true;
  // Путь к базе данных для всех префиксов
  std::string path = "./rocksdb";
  // Файл опций в формате ini
  std::string ini = "./rocksdb.ini";
  // Путь к бэкапу базы данных для всех префиксов
  std::string backup_path  = "./rocksdb/backup";
  
};

}}
