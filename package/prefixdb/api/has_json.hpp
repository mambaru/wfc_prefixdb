#pragma once

#include <prefixdb/api/has.hpp>
#include <prefixdb/api/fields_json.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct has_json
  {
    JSON_NAME(prefix)
    JSON_NAME(fields)
    JSON_NAME(snapshot)
    typedef wfc::json::object<
      has,
      wfc::json::member_list<
        wfc::json::member<n_snapshot,  has, size_t, &has::snapshot>,
        wfc::json::member<n_prefix, has, std::string, &has::prefix>,
        wfc::json::member<n_fields, has, key_list_t, &has::fields, key_list_json >
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

    typedef wfc::json::object<
      has,
      wfc::json::member_list<
        wfc::json::member<n_prefix, has, std::string, &has::prefix>,
        wfc::json::member<n_status, has, common_status, &has::status, common_status_json>,
        wfc::json::member<n_fields, has, field_list_t, &has::fields, fields_list_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
