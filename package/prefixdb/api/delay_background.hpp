#pragma once

#include <prefixdb/api/aux/common_status.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  struct delay_background
  {
    typedef std::vector<std::string> prefix_list;
    // Пустой список - все префиксы
    prefix_list prefixes;
    time_t delay_timeout_s = 600;
    bool contunue_force = false;
    typedef std::unique_ptr<delay_background> ptr;
  };
}

namespace response
{
  struct delay_background
  {
    
    common_status status = common_status::OK;
    typedef std::unique_ptr<delay_background> ptr;
    typedef std::function< void(ptr) > handler;
  };
}

}}
