#pragma once


#include <prefixdb/iprefixdb.hpp>
#include <wfc/workflow.hpp>
#include <memory>
#include <string>
#include <ctime>

namespace wamba{ namespace prefixdb{
  
struct slave_config
{
  bool enabled = false;
  
  std::string target;
  std::string start_time;
  
  // интервал запросов к мастеру в милисекундах 
  time_t pull_timeout_ms   = 1000;
  time_t query_prefixes_timeout_ms = 2000;
  // ограничение на количество элементов в ответе на один запрос к мастеру
  size_t log_limit_per_req = 100;
  // отображать в логе процесс загрузки 
  bool enable_progress = false;
  // выдерживать интервал после каждого запроса. В противном случае, если не достигнут конец лога, следующий запрос
  bool expires_for_req = true;
  // допустимые потери последовательностей. При превышении этого значения демон завершает работу
  std::ptrdiff_t acceptable_loss_seq = 0;
  // при каком отставании ругатся в логи
  std::ptrdiff_t wrn_log_diff_seq = 10000;
  size_t seq_log_timeout_ms = 1000;
  std::shared_ptr<iprefixdb> master;
  std::shared_ptr< ::wfc::workflow > timer;
};


}}
