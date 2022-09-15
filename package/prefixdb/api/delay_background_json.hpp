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
    
    typedef wjson::object<
      delay_background,
      wjson::member_list<
        wjson::member<n_prefixes, delay_background, delay_background::prefix_list, &delay_background::prefixes, wjson::vector_of_strings<50> >,
        wjson::member<n_delay_timeout_s, delay_background, time_t, &delay_background::delay_timeout_s >,
        wjson::member<n_contunue_force, delay_background, bool, &delay_background::contunue_force >
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

namespace response
{
  struct delay_background_json
  {
    JSON_NAME(status)

    typedef wjson::object<
      delay_background,
      wjson::member_list<
        wjson::member<n_status, delay_background, common_status, &delay_background::status, common_status_json>
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

}}
