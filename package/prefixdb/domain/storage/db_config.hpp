#pragma once

#include <string>
//#include <prefixdb/domain/storage/merge/merge_config.hpp>
#include <prefixdb/iprefixdb.hpp>
//#include <iow/io/timer/timer.hpp>
#include <wfc/workflow.hpp>
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
  // допустимые потери последовательностей. При превышении этого значения демон завершает работу
  size_t acceptable_loss_seq = 0;
  // при каком отставании ругатся в логи
  size_t wrn_log_diff_seq = 10000;
  size_t seq_log_timeout_ms = 1000;
  std::shared_ptr<iprefixdb> master;
  std::shared_ptr< ::wfc::workflow > timer;
};

struct backup_config
{
  bool enabled = false;
  
  // Путь к бэкапу базы данных для всех префиксов
  std::string path   = "./prefixdb_backup";
  
  std::string start_time  = "04:00:00";
  
  time_t period_s = 0;
  
  // не реализовано
  time_t start_delay_s = 0;
  
};

struct archive_config
{
  bool enabled = false;

  // архив бэкапов
  std::string path  = "./prefixdb_archive";

  std::string start_time = "05:00:00";
  
  time_t period_s = 0;

};

// 
struct restore_config
{
  // запретить востановление для данного конфига
  bool forbid = false;

  // архив бэкапов
  std::string path  = "./prefixdb_archive";
};

struct db_config
{
  size_t packed_limit = 1000;
  size_t array_limit  = 1000;

  // Путь к базе данных для всех префиксов
  std::string path = "./prefixdb";
  // Файл опций в формате ini
  std::string ini = "./rocksdb.ini";
  // Путь к бэкапу базы данных для всех префиксов
  // std::string backup_path   = "./prefixdb_backup";
  // Откуда востанавливаться в случае сбоя 
  std::string restore_path  = "./prefixdb_backup";
  
  backup_config backup;
  archive_config archive;
  restore_config restore;
  // bool compact_before_backup = false;
  
  slave_config slave;
  //master_config master;
};

}}
