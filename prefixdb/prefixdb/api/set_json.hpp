#pragma once

#include <prefixdb/api/set.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct set_json
  {
    JSON_NAME(prefix)
    JSON_NAME(force)
    JSON_NAME(nores)
    JSON_NAME(noval)
    JSON_NAME(fields)

    typedef ::wfc::json::object<
      set::field,
      ::wfc::json::member_list<
        ::wfc::json::base< basic_field_json >,
        ::wfc::json::member<n_force, set::field, bool, &set::field::force>
      >
    > field_json;
    typedef ::wfc::json::array< std::vector< field_json > > array_of_fields_json;

    typedef ::wfc::json::object<
      set,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefix, set, std::string, &set::prefix>,
        ::wfc::json::member<n_nores,  set, bool, &set::nores>,
        ::wfc::json::member<n_noval,  set, bool, &set::noval>,
        ::wfc::json::member<n_fields, set, set::field_list_t, &set::fields, array_of_fields_json>
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

    typedef ::wfc::json::object<
      set::field,
      ::wfc::json::member_list<
        ::wfc::json::base< field_base_json >
      >
    > field_json;
    typedef ::wfc::json::array< std::vector< field_json > > array_of_fields_json;

    typedef ::wfc::json::object<
      set,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefix, set, std::string, &set::prefix>,
        ::wfc::json::member<n_status, set, common_status, &set::status, common_status_json>,
        ::wfc::json::member<n_fields, set, set::field_list_t, &set::fields, array_of_fields_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
