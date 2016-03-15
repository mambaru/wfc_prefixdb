#pragma once

#include <string>
#include <prefixdb/domain/storage/merge/merge_config.hpp>
#include <prefixdb/iprefixdb.hpp>
#include <iow/io/timer/timer.hpp>
#include <memory>

namespace wamba{ namespace prefixdb{
  
struct slave_config
{
  bool enabled = false;
  
  std::string target;
  std::string start_time;
  
  // интервал запросов к мастеру в милисекундах 
  time_t pull_timeout_ms   = 1000;
  // ограничение на количество элементов в ответе на один запрос к мастеру
  size_t log_limit_per_req = 100;
  // отображать в логе процесс загрузки 
  bool enable_progress = false;
  // выдерживать интервал после каждого запроса. В противном случае, если не достигнут конец лога, следующий запрос
  bool expires_for_req = true;
  
  std::shared_ptr<iprefixdb> master;
  std::shared_ptr< ::iow::io::timer> timer;
};

struct master_config
{
  
};

struct rocksdb_config: merge_config
{
  // Путь к базе данных для всех префиксов
  std::string path = "./prefixdb";
  // Файл опций в формате ini
  std::string ini = "./rocksdb.ini";
  // Путь к бэкапу базы данных для всех префиксов
  std::string backup_path   = "./prefixdb_backup";
  // Откуда востанавливаться в случае сбоя 
  std::string restore_path  = "./prefixdb_backup";
  // архив бэкапов
  std::string archive_path  = "./prefixdb_archive";
  
  slave_config slave;
  master_config master;
};

}}
