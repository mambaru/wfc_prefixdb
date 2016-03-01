#pragma once

#include <prefixdb/api/aux/common_status.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  struct get_updates_since
  {
    std::string prefix;
    size_t seq = 0;
    size_t limit = 10;
    typedef std::unique_ptr<get_updates_since> ptr;
  };
}

namespace response
{
  struct get_updates_since
  {
    std::string prefix;
    size_t seq_first = 0;
    size_t seq_last = 0;
    size_t seq_final = 0;
    std::vector< std::string > logs;
    common_status status =  common_status::OK;

    typedef std::unique_ptr<get_updates_since> ptr;
    typedef std::function< void(ptr) > handler;
  };

}

}}
