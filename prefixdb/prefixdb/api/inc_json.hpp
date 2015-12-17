#pragma once

#include <prefixdb/api/inc.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct inc_json
  {
    JSON_NAME(prefix)
    JSON_NAME(def)
    JSON_NAME(nores)
    JSON_NAME(fields)

    typedef ::wfc::json::object<
      inc::field,
      ::wfc::json::member_list<
        ::wfc::json::base< basic_field_json >,
        ::wfc::json::member<n_def, inc::field, std::string, &inc::field::def>
      >
    > field_json;
    typedef ::wfc::json::array< std::vector< field_json > > array_of_fields_json;

    typedef ::wfc::json::object<
      inc,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefix, inc, std::string, &inc::prefix>,
        ::wfc::json::member<n_nores,  inc, bool, &inc::nores>,
        ::wfc::json::member<n_fields, inc, inc::field_list_t, &inc::fields, array_of_fields_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

namespace response
{
  struct inc_json
  {
    JSON_NAME(prefix)
    JSON_NAME(status)
    JSON_NAME(fields)

    typedef ::wfc::json::object<
      inc::field,
      ::wfc::json::member_list<
        ::wfc::json::base< field_base_json >
      >
    > field_json;
    typedef ::wfc::json::array< std::vector< field_json > > array_of_fields_json;

    typedef ::wfc::json::object<
      inc,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefix, inc, std::string, &inc::prefix>,
        ::wfc::json::member<n_status, inc, common_status, &inc::status, common_status_json>,
        ::wfc::json::member<n_fields, inc, inc::field_list_t, &inc::fields, array_of_fields_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
