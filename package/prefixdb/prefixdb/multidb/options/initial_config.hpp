#pragma once


#include <prefixdb/iprefixdb.hpp>
#include <wfc/workflow.hpp>
#include <memory>
#include <string>
#include <ctime>

namespace wamba{ namespace prefixdb{
  
struct initial_config
{
  bool enabled = false;
  bool disableWAL = false;
  bool use_setnx = true;
  size_t initial_range = 1024;
  
  std::shared_ptr<iprefixdb> remote;
  std::shared_ptr<iprefixdb> local;
};


}}
