#pragma once

#include <prefixdb/api/aux/common_status.hpp>
#include <memory>
#include <string>
#include <functional>

namespace wamba { namespace prefixdb {

namespace request
{
  struct release_snapshot
  {
    std::string prefix;
    size_t snapshot = 0;
    typedef std::unique_ptr<release_snapshot> ptr;
  };
}

namespace response
{
  struct release_snapshot
  {
    std::string prefix;
    common_status status = common_status::OK;
    typedef std::unique_ptr<release_snapshot> ptr;
    typedef std::function< void(ptr) > handler;
  };
}

}}
