#pragma once

#include <prefixdb/api/get.hpp>
#include <prefixdb/api/aux/basic_field_json.hpp>
#include <prefixdb/api/aux/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct get_json
  {
    JSON_NAME(prefix)
    JSON_NAME(fields)
    typedef ::wfc::json::object<
      get,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefix, get, std::string, &get::prefix>,
        ::wfc::json::member<n_fields, get, get::field_list_t, &get::fields, ::wfc::json::array< std::vector<key_field_json> > >
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };

}

namespace response
{
  struct get_json
  {
    JSON_NAME(prefix)
    JSON_NAME(status)
    JSON_NAME(fields)

    typedef ::wfc::json::object<
      get::field,
      ::wfc::json::member_list<
        ::wfc::json::base< basic_field_json >
      >
    > field_json;
    typedef ::wfc::json::array< std::vector< field_json > > array_of_fields_json;

    typedef ::wfc::json::object<
      get,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefix, get, std::string, &get::prefix>,
        ::wfc::json::member<n_status, get, common_status, &get::status, common_status_json>,
        ::wfc::json::member<n_fields, get, get::field_list_t, &get::fields, array_of_fields_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
