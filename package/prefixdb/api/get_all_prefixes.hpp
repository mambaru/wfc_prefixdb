#pragma once

#include <prefixdb/api/aux/common_status.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  struct get_all_prefixes
  {
    typedef std::unique_ptr<get_all_prefixes> ptr;
  };
}

namespace response
{
  struct get_all_prefixes
  {
    typedef std::vector<std::string> prefix_list;
    prefix_list prefixes;
    common_status status =  common_status::OK;

    typedef std::unique_ptr<get_all_prefixes> ptr;
    typedef std::function< void(ptr) > handler;
  };

}

}}
