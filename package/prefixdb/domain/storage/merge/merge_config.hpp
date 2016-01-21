#pragma once

namespace wamba{ namespace prefixdb{

struct merge_config
{
  size_t packed_limit = 1000;
  size_t array_limit  = 1000;
  size_t json_limit   = 1000;
};


}}
