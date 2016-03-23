#pragma once

#include <prefixdb/api/get_all_prefixes.hpp>
#include <prefixdb/api/aux/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct get_all_prefixes_json
  {
    typedef ::wfc::json::object<
      get_all_prefixes,
      ::wfc::json::member_list<
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

    typedef ::wfc::json::object<
      get_all_prefixes,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefixes, get_all_prefixes, get_all_prefixes::prefix_list, &get_all_prefixes::prefixes, wfc::json::array_of_strings<50> >
        
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
