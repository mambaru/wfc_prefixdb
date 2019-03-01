#pragma once

#include <prefixdb/api/continue_background.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct continue_background_json
  {
    JSON_NAME(prefixes)
    JSON_NAME(background_id)
    JSON_NAME(force)
    
    typedef wfc::json::object<
      continue_background,
      wfc::json::member_list<
        wfc::json::member<n_prefixes, continue_background, continue_background::prefix_list, &continue_background::prefixes, wfc::json::vector_of_strings<50> >,
        wfc::json::member<n_force, continue_background, bool, &continue_background::force >
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

namespace response
{
  struct continue_background_json
  {
    JSON_NAME(status)

    typedef wfc::json::object<
      continue_background,
      wfc::json::member_list<
        wfc::json::member<n_status, continue_background, common_status, &continue_background::status, common_status_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
