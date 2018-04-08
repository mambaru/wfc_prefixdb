#pragma once

#include <prefixdb/api/compact_prefix.hpp>
#include <prefixdb/api/aux/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct compact_prefix_json
  {
    JSON_NAME(prefix)
    JSON_NAME(from)
    JSON_NAME(to)
    
    typedef ::wfc::json::object<
      compact_prefix,
        wfc::json::member_list<
          wfc::json::member<n_prefix, compact_prefix, std::string, &compact_prefix::prefix >,
          wfc::json::member<n_from, compact_prefix, std::string, &compact_prefix::from>,
          wfc::json::member<n_to, compact_prefix, std::string, &compact_prefix::to>
        >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

namespace response
{
  struct compact_prefix_json
  {
    JSON_NAME(status)
    JSON_NAME(prefix)

    typedef ::wfc::json::object<
      compact_prefix,
      wfc::json::member_list<
        wfc::json::member<n_prefix, compact_prefix, std::string, &compact_prefix::prefix>,
        wfc::json::member<n_status, compact_prefix, common_status, &compact_prefix::status, common_status_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
