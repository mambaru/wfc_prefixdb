#pragma once

#include <prefixdb/api/common_status.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  struct detach_prefixes
  {
    typedef std::vector<std::string> prefix_list;
    prefix_list prefixes;
    time_t deny_timeout_s = 3600;
    typedef std::unique_ptr<detach_prefixes> ptr;
  };
}

namespace response
{
  struct detach_prefixes
  {
    common_status status =  common_status::OK;
    typedef std::unique_ptr<detach_prefixes> ptr;
    typedef std::function< void(ptr) > handler;
  };

}

}}
