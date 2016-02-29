#pragma once

#include <string>
#include <prefixdb/domain/storage/merge/merge_config.hpp>

namespace wamba{ namespace prefixdb{
  
struct rocksdb_config: merge_config
{
  // Путь к базе данных для всех префиксов
  std::string path = "./rocksdb";
  // Файл опций в формате ini
  std::string ini = "./rocksdb.ini";
  // Путь к бэкапу базы данных для всех префиксов
  std::string backup_path   = "./rocksdb_backup";
  std::string restore_path  = "./rocksdb_backup";
};

}}
