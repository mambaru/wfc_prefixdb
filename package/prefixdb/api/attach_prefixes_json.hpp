#pragma once

#include <prefixdb/api/attach_prefixes.hpp>
#include <prefixdb/api/aux/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct attach_prefixes_json
  {
    JSON_NAME(prefixes)
    JSON_NAME(opendb)
    
    typedef ::wfc::json::object<
      attach_prefixes,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefixes, attach_prefixes, attach_prefixes::prefix_list, &attach_prefixes::prefixes, wfc::json::vector_of_strings<50> >,
        ::wfc::json::member<n_opendb, attach_prefixes, bool, &attach_prefixes::opendb >
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

namespace response
{
  struct attach_prefixes_json
  {
    JSON_NAME(status)

    typedef ::wfc::json::object<
      attach_prefixes,
      ::wfc::json::member_list<
        ::wfc::json::member<n_status, attach_prefixes, common_status, &attach_prefixes::status, common_status_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
