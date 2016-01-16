#pragma once

#include <string>

namespace wamba{ namespace prefixdb{

enum class merge_mode
{
  inc,
  packed
};

struct merge
{
  merge_mode mode;
  std::string raw;
};


}}
