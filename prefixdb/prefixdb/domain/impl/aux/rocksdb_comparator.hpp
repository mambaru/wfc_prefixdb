#pragma once

#include "comparator.hpp"
#include <rocksdb/comparator.h>
#include <rocksdb/slice.h>

namespace wamba{ namespace prefixdb{

struct rocksdb_comparator
  : comparator< 
      std::string, 
      std::less<std::string>, 
      ::rocksdb::Comparator, 
      ::rocksdb::Slice
    > 
{};

}}