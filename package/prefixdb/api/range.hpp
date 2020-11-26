#pragma once

#include <prefixdb/api/common_status.hpp>
#include <prefixdb/api/fields.hpp>
#include <wrtstat/aggregator/api/aggregated_data.hpp>
#include <wrtstat/aggregator/api/aggregated_info.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  struct range
  {
    std::string prefix;
    bool nores = false; // Установить в true для stat=true
    bool noval = false; // TODO: true, после реализации nores
    bool stat = false;  // собирать стату

    std::string from;
    std::string to;
    bool beg = true;
    bool repair_json = false; // Принудительный repair-JSON не зависимо от опции в конфиге (для слейва)
    size_t offset = 0;
    size_t limit = 10;
    size_t snapshot = 0;
    typedef std::unique_ptr<range> ptr;
  };
}

namespace response
{
  struct range
  {
    struct stat_info
    {
      size_t null_count = 0;
      size_t bool_count = 0;
      size_t number_count = 0;
      size_t string_count = 0;
      size_t array_count = 0;
      size_t object_count = 0;
      size_t repair_count = 0;
      size_t empty_count = 0;

      wrtstat::aggregated_info keys;
      wrtstat::aggregated_info values;
      typedef std::shared_ptr<stat_info> ptr;
    };

    bool fin = true;
    common_status status = common_status::OK ;
    std::string prefix;
    field_list_t fields;
    stat_info::ptr stat;

    typedef std::unique_ptr<range> ptr;
    typedef std::function< void(ptr) > handler;
  };
}

}}
