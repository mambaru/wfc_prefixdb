#pragma once

#include <prefixdb/api/detach_prefixes.hpp>
#include <prefixdb/api/aux/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct detach_prefixes_json
  {
    JSON_NAME(prefixes)
    JSON_NAME(deny_timeout_s)
    
    typedef wfc::json::object<
      detach_prefixes,
      wfc::json::member_list<
        wfc::json::member<n_prefixes, detach_prefixes, detach_prefixes::prefix_list, &detach_prefixes::prefixes, wfc::json::vector_of_strings<50> >,
        wfc::json::member<n_deny_timeout_s, detach_prefixes, time_t, &detach_prefixes::deny_timeout_s >
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

namespace response
{
  struct detach_prefixes_json
  {
    JSON_NAME(status)

    typedef wfc::json::object<
      detach_prefixes,
      wfc::json::member_list<
        wfc::json::member<n_status, detach_prefixes, common_status, &detach_prefixes::status, common_status_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
