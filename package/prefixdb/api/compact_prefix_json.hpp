#pragma once

#include <prefixdb/api/compact_prefix.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct compact_prefix_json
  {
    JSON_NAME(prefix)
    JSON_NAME(from)
    JSON_NAME(to)
    
    typedef wjson::object<
      compact_prefix,
        wjson::member_list<
          wjson::member<n_prefix, compact_prefix, std::string, &compact_prefix::prefix >,
          wjson::member<n_from, compact_prefix, std::string, &compact_prefix::from>,
          wjson::member<n_to, compact_prefix, std::string, &compact_prefix::to>
        >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

namespace response
{
  struct compact_prefix_json
  {
    JSON_NAME(status)
    JSON_NAME(prefix)

    typedef wjson::object<
      compact_prefix,
      wjson::member_list<
        wjson::member<n_prefix, compact_prefix, std::string, &compact_prefix::prefix>,
        wjson::member<n_status, compact_prefix, common_status, &compact_prefix::status, common_status_json>
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

}}
