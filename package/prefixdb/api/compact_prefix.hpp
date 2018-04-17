#pragma once

#include <prefixdb/api/common_status.hpp>
#include <memory>
#include <string>
#include <functional>

namespace wamba { namespace prefixdb {

namespace request
{
  // compact для конкретного префикса
  struct compact_prefix
  {
    // Пустой prefix - ошибка
    std::string prefix;
    std::string from;
    std::string to;
    typedef std::unique_ptr<compact_prefix> ptr;
  };
}

namespace response
{
  struct compact_prefix
  {
    std::string prefix;
    common_status status = common_status::OK;
    typedef std::unique_ptr<compact_prefix> ptr;
    typedef std::function< void(ptr) > handler;
  };
}

}}
