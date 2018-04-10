#pragma once

#include <prefixdb/api/packed.hpp>
#include <prefixdb/api/add.hpp>
#include <prefixdb/api/aux/fields_json.hpp>
#include <prefixdb/api/aux/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct packed_json
  {
    JSON_NAME(prefix)
    JSON_NAME(nores)
    JSON_NAME(update)
    JSON_NAME(sync)

    typedef wfc::json::object<
      packed,
      wfc::json::member_list<
        wfc::json::member<n_prefix, packed, std::string, &packed::prefix>,
        wfc::json::member<n_sync,   packed, bool, &packed::sync>,
        wfc::json::member<n_nores,  packed, bool, &packed::nores>,
        wfc::json::member<n_update, packed, packed::field_list_t, &packed::fields, raw_fields_list_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

namespace response
{
  struct packed_json
  {
    JSON_NAME(prefix)
    JSON_NAME(status)
    JSON_NAME(fields)

    typedef wfc::json::object<
      packed,
      wfc::json::member_list<
        wfc::json::member<n_prefix, packed, std::string, &packed::prefix>,
        wfc::json::member<n_fields, packed, packed::field_list_t, &packed::fields, raw_fields_list_json>,
        wfc::json::member<n_status, packed, common_status, &packed::status, common_status_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
