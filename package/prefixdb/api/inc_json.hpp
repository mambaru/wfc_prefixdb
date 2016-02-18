#pragma once

#include <prefixdb/api/inc.hpp>
#include <prefixdb/api/add.hpp>
#include <prefixdb/api/aux/fields_json.hpp>
#include <prefixdb/api/aux/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct inc_json
  {
    JSON_NAME(prefix)
    JSON_NAME(sync)
    JSON_NAME(nores)
    JSON_NAME(update)

    typedef ::wfc::json::object<
      inc,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefix, inc, std::string, &inc::prefix>,
        ::wfc::json::member<n_sync,   inc, bool, &inc::sync>,
        ::wfc::json::member<n_nores,  inc, bool, &inc::nores>,
        ::wfc::json::member<n_update, inc, inc::field_list_t, &inc::fields, raw_fields_list_json>
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
      inc,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefix, inc, std::string, &inc::prefix>,
        ::wfc::json::member<n_status, inc, common_status, &inc::status, common_status_json>,
        ::wfc::json::member<n_fields, inc, inc::field_list_t, &inc::fields, raw_fields_list_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
