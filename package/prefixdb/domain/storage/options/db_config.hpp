#pragma once

#include <string>
#include <prefixdb/domain/storage/options/slave_config.hpp>
//#include <prefixdb/domain/storage/options/master_config.hpp>
#include <prefixdb/domain/storage/options/backup_config.hpp>
#include <prefixdb/domain/storage/options/archive_config.hpp>
#include <prefixdb/domain/storage/options/restore_config.hpp>

namespace wamba{ namespace prefixdb{
  
struct db_config
{
  size_t packed_limit = 1000;
  size_t array_limit  = 1000;
  size_t range_limit  = 10000; // offset + limit

  // Путь к базе данных для всех префиксов
  std::string path = "./prefixdb";
  // для "отцепленных" префиксов
  std::string detach_path = "./prefixdb_detach";
  // Файл опций в формате ini
  std::string ini = "./rocksdb.ini";
  
  // Автоматически попытаться востановить базу при ошибке открытия
  bool auto_repair = false;

  // Завершение работы, если база префикса не смогла быть открыта
  bool abort_if_open_error = true;

  // Запись batch в отдельном потоке
  bool enable_delayed_write = false;  
  
  //
  bool check_merge_operations = true;
  
  //master_config master;
  slave_config slave;
  backup_config backup;
  archive_config archive;
  restore_config restore;
  
  struct { std::shared_ptr< ::wfc::workflow > workflow; } args;
  
  /*
  ::wfc::workflow_options workflow;
  
  std::shared_ptr< ::wfc::workflow > workflow_ptr;
  */
};

}}
