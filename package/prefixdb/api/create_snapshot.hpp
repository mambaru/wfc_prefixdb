#pragma once

#include <prefixdb/api/aux/common_status.hpp>
#include <memory>
#include <string>
#include <functional>

namespace wamba { namespace prefixdb {

namespace request
{
  struct create_snapshot
  {
    std::string prefix;
    time_t release_timeout_s = 3600;
    typedef std::unique_ptr<create_snapshot> ptr;
  };
}

namespace response
{
  struct create_snapshot
  {
    size_t snapshot = 0;
    std::string prefix;
    common_status status = common_status::OK;
    typedef std::unique_ptr<create_snapshot> ptr;
    typedef std::function< void(ptr) > handler;
  };
}

}}
