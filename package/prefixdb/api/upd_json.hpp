#pragma once

#include <prefixdb/api/upd.hpp>
#include <prefixdb/api/aux/key_field_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct upd_json
  {
    JSON_NAME(prefix)
    JSON_NAME(key)
    JSON_NAME(inc)
    JSON_NAME(def)
    JSON_NAME(val)
    JSON_NAME(nores)
    JSON_NAME(noval)
    JSON_NAME(fields)
    JSON_NAME(sync)

    typedef ::wfc::json::object<
      upd::params,
      ::wfc::json::member_list<
        ::wfc::json::base<key_field_json>,
        ::wfc::json::member<n_inc, upd::params, int, &upd::params::inc>,
        ::wfc::json::member<n_def, upd::params, int, &upd::params::def>
      >
    > params_json;
    typedef ::wfc::json::array< std::vector< params_json > > array_of_params_json;

    
    typedef ::wfc::json::object<
      upd::field,
      ::wfc::json::member_list<
        ::wfc::json::base< field_base_json >,
        ::wfc::json::member<n_val, upd::field, upd::field::params_list_t, &upd::field::val, array_of_params_json>
      >
    > field_json;
    typedef ::wfc::json::array< std::vector< field_json > > array_of_fields_json;

    typedef ::wfc::json::object<
      upd,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefix, upd, std::string, &upd::prefix>,
        ::wfc::json::member<n_nores,  upd, bool, &upd::nores>,
        ::wfc::json::member<n_sync,   upd, bool, &upd::sync>,
        ::wfc::json::member<n_fields, upd, upd::field_list_t, &upd::fields, array_of_fields_json>
      >
    > type;

    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

namespace response
{
  struct upd_json
  {
    JSON_NAME(prefix)
    JSON_NAME(status)
    JSON_NAME(fields)

    typedef ::wfc::json::object<
      upd::field,
      ::wfc::json::member_list<
        ::wfc::json::base< basic_field_json >
      >
    > field_json;
    typedef ::wfc::json::array< std::vector< field_json > > array_of_fields_json;

    typedef ::wfc::json::object<
      upd,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefix, upd, std::string, &upd::prefix>,
        ::wfc::json::member<n_status, upd, common_status, &upd::status, common_status_json>,
        ::wfc::json::member<n_fields, upd, upd::field_list_t, &upd::fields, array_of_fields_json>
      >
    > type;
    
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
