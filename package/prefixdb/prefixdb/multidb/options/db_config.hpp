#pragma once

#include <string>
#include <prefixdb/prefixdb/multidb/options/slave_config.hpp>
#include <prefixdb/prefixdb/multidb/options/backup_config.hpp>
#include <prefixdb/prefixdb/multidb/options/archive_config.hpp>
#include <prefixdb/prefixdb/multidb/options/restore_config.hpp>
#include <prefixdb/prefixdb/multidb/options/compact_config.hpp>
#include <prefixdb/prefixdb/multidb/options/initial_config.hpp>

namespace wamba{ namespace prefixdb{
  
struct db_config
{

  // Путь к базе данных для всех префиксов
  std::string path = "";
  // Путь для WAL, если не указан то по умолчанью из rocksdb
  // если путь указано в ini, то wal_path + "prefix" + ini.wal_dir
  std::string wal_path = "";
  // для "отцепленных" префиксов
  std::string detach_path = "";
  // Файл опций в формате ini
  std::string ini = "";
  
  uint32_t TTL_seconds = 0;
  size_t packed_limit = 0;
  size_t array_limit  = 0;
  // range.limit + range.offset, для защиты от перебора. Нужно использовать from и to
  size_t range_limit  = 0; 
  
  // Автоматически попытаться востановить базу при ошибке открытия
  bool auto_repair = false;
  // Восстанавливать даже если открывается (только через параметры командной строки)
  bool forced_repair = false;

  // Завершение работы, если база префикса не смогла быть открыта
  bool abort_if_open_error = true;

  // Запись batch в отдельном потоке
  bool enable_delayed_write = false;  
  
  // Проверять на валидность параметры JSON в merge - опирациях 
  // (параметров в WAL идет сыром виде, если там мусор будет ошибка, 
  //  котору будет трудно идентифицировать )
  bool check_incoming_merge_json = true;
  
  compact_config compact;
  slave_config slave;
  initial_config initial_load;
  backup_config backup;
  archive_config archive;
  restore_config restore;
  
  struct 
  {
    std::shared_ptr<wfc::workflow> timers_workflow;
    std::shared_ptr<wfc::workflow> write_workflow; 
  } args;
};

}}
