#pragma once

#include <prefixdb/api/common_status.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  struct continue_background
  {
    typedef std::vector<std::string> prefix_list;
    // Пустой список - все префиксы
    prefix_list prefixes;
    bool force = false;
    typedef std::unique_ptr<continue_background> ptr;
  };
}

namespace response
{
  struct continue_background
  {
    common_status status = common_status::OK;
    typedef std::unique_ptr<continue_background> ptr;
    typedef std::function< void(ptr) > handler;
  };
}

}}
