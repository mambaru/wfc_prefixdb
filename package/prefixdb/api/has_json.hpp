#pragma once

#include <prefixdb/api/has.hpp>
#include <prefixdb/api/aux/fields_json.hpp>
#include <prefixdb/api/aux/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct has_json
  {
    JSON_NAME(prefix)
    JSON_NAME(fields)
    typedef ::wfc::json::object<
      has,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefix, has, std::string, &has::prefix>,
        ::wfc::json::member<n_fields, has, has::field_list_t, &has::fields, key_list_json >
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };

}

namespace response
{
  struct has_json
  {
    JSON_NAME(prefix)
    JSON_NAME(status)
    JSON_NAME(fields)

    /*
    typedef ::wfc::json::object<
      has::field,
      ::wfc::json::member_list<
        ::wfc::json::base< field_base_json >
      >
    > field_json;
    typedef ::wfc::json::array< std::vector< field_json > > array_of_fields_json;
    */

    typedef ::wfc::json::object<
      has,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefix, has, std::string, &has::prefix>,
        ::wfc::json::member<n_status, has, common_status, &has::status, common_status_json>,
        ::wfc::json::member<n_fields, has, has::field_list_t, &has::fields, raw_fields_list_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
