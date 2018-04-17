#pragma once

#include <prefixdb/api/delay_background.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct delay_background_json
  {
    JSON_NAME(prefixes)
    JSON_NAME(delay_timeout_s)
    JSON_NAME(contunue_force)
    
    typedef wfc::json::object<
      delay_background,
      wfc::json::member_list<
        wfc::json::member<n_prefixes, delay_background, delay_background::prefix_list, &delay_background::prefixes, wfc::json::vector_of_strings<50> >,
        wfc::json::member<n_delay_timeout_s, delay_background, time_t, &delay_background::delay_timeout_s >,
        wfc::json::member<n_contunue_force, delay_background, bool, &delay_background::contunue_force >
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

namespace response
{
  struct delay_background_json
  {
    JSON_NAME(status)

    typedef wfc::json::object<
      delay_background,
      wfc::json::member_list<
        wfc::json::member<n_status, delay_background, common_status, &delay_background::status, common_status_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
