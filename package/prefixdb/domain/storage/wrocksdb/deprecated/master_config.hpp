#pragma once


#include <prefixdb/domain/storage/wrocksdb/wal_buffer.hpp>
#include <wfc/workflow.hpp>
#include <memory>
#include <string>
#include <ctime>

namespace wamba{ namespace prefixdb{
  
struct master_config
{
  bool enabled = false;
  size_t log_buffer_size = 10000;
  std::shared_ptr<wal_buffer> walbuf;
};


}}
