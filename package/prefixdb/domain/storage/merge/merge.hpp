#pragma once

#include <string>

namespace wamba{ namespace prefixdb{

enum class merge_mode
{
  none,
  inc,
  add,
  packed
};

struct merge
{
  merge_mode mode = merge_mode::none;
  std::string raw;
};


}}
