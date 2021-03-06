#pragma once


#include <prefixdb/iprefixdb.hpp>
#include <wfc/workflow.hpp>
#include <memory>
#include <string>
#include <vector>
#include <ctime>

namespace wamba{ namespace prefixdb{

struct slave_config
{
  bool enabled = false;

  std::string target;
  std::string start_time;

  // вспомогательные файлы репликации (last_sequence_number)
  std::string path;
  
  // Реплицировать только префиксы разрешенные для записи на мастере
  bool writable_only = false;
  // Разрешенные префиксы для репликации
  std::vector<std::string> allowed_prefixes;
  // Запрещенные префиксы для репликации
  std::vector<std::string> denied_prefixes;

  // интервал запросов к мастеру в милисекундах
  time_t pull_timeout_ms           = 1000;
  time_t query_prefixes_timeout_ms = 2000;
  // ограничение на количество элементов в ответе на один запрос к мастеру
  size_t log_limit_per_req = 100;
  // отображать в логе процесс загрузки
  // bool enable_progress = false;
  // выдерживать интервал после каждого запроса. В противном случае, если не достигнут конец лога, следующий запрос
  bool expires_for_req = false;
  // допустимые потери последовательностей. При превышении этого значения демон завершает работу
  std::ptrdiff_t acceptable_loss_seq = 0;
  // при каком отставании ругатся в логи
  std::ptrdiff_t wrn_log_diff_seq = 10000;
  time_t wrn_log_timeout_ms = 1000;
  time_t seq_log_timeout_ms = 1000;
  // Отключить WAL для обновлений
  bool disableWAL = false;

  std::shared_ptr<iprefixdb> master;
  std::shared_ptr<wflow::workflow> timer;
};


}}
