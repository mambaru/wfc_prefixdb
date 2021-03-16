#pragma once

#include <prefixdb/api/get_all_prefixes.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct get_all_prefixes_json
  {
    typedef wjson::object<
      get_all_prefixes,
      wjson::member_list<
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

namespace response
{
  struct get_all_prefixes_json
  {
    JSON_NAME(prefixes)
    JSON_NAME(status)

    typedef wjson::object<
      get_all_prefixes,
      wjson::member_list<
        wjson::member<n_prefixes, get_all_prefixes, get_all_prefixes::prefix_list, &get_all_prefixes::prefixes, wjson::vector_of_strings<50> >,
        wjson::member<n_status, get_all_prefixes, common_status, &get_all_prefixes::status, common_status_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
