#pragma once

#include <prefixdb/api/set.hpp>
#include <prefixdb/api/aux/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

typedef ::wfc::json::object2array<
  ::wfc::json::value<std::string>,
  ::wfc::json::raw_value<std::string>,
  10
> raw_fields_list_json;

typedef ::wfc::json::array< std::vector< ::wfc::json::value<std::string> >, 10 > key_list_json;


namespace request 
{
  struct set_json
  {
    JSON_NAME(prefix)
    //JSON_NAME(force)
    JSON_NAME(nores)
    JSON_NAME(noval)
    JSON_NAME(fields)
    JSON_NAME(sync)

    /*
    typedef ::wfc::json::object<
      set::field,
      ::wfc::json::member_list<
        ::wfc::json::base< basic_field_json >,
        ::wfc::json::member<n_force, set::field, bool, &set::field::force>
      >
    > field_json;
    typedef ::wfc::json::array< std::vector< field_json >, 10 > array_of_fields_json;
    */

    typedef ::wfc::json::object<
      set,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefix, set, std::string, &set::prefix>,
        ::wfc::json::member<n_sync,   set, bool, &set::sync>,
        ::wfc::json::member<n_nores,  set, bool, &set::nores>,
        ::wfc::json::member<n_noval,  set, bool, &set::noval>,
        ::wfc::json::member<n_fields, set, set::field_list_t, &set::fields, raw_fields_list_json >
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

namespace response
{
  struct set_json
  {
    JSON_NAME(prefix)
    JSON_NAME(status)
    JSON_NAME(fields)

    /*
    typedef ::wfc::json::object<
      set::field,
      ::wfc::json::member_list<
        ::wfc::json::base< basic_field_json >
      >
    > field_json;
    typedef ::wfc::json::array< std::vector< field_json > > array_of_fields_json;
    */

    typedef ::wfc::json::object<
      set,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefix, set, std::string, &set::prefix>,
        ::wfc::json::member<n_status, set, common_status, &set::status, common_status_json>,
        ::wfc::json::member<n_fields, set, set::field_list_t, &set::fields, raw_fields_list_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
