#pragma once

#include <prefixdb/api/aux/common_status.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  struct attach_prefixes
  {
    typedef std::vector<std::string> prefix_list;
    prefix_list prefixes;
    bool opendb = false;
    typedef std::unique_ptr<attach_prefixes> ptr;
  };
}

namespace response
{
  struct attach_prefixes
  {
    common_status status =  common_status::OK;
    typedef std::unique_ptr<attach_prefixes> ptr;
    typedef std::function< void(ptr) > handler;
  };

}

}}
