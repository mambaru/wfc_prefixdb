#pragma once

#include <prefixdb/api/add.hpp>
#include <prefixdb/api/fields_json.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct add_json
  {
    JSON_NAME(prefix)
    JSON_NAME(force)
    JSON_NAME(nores)
    JSON_NAME(noval)
    JSON_NAME(sync)
    JSON_NAME(update)
    JSON_NAME(snapshot)
    
    typedef wfc::json::object<
      add,
      wfc::json::member_list<
        wfc::json::member<n_prefix, add, std::string, &add::prefix>,
        wfc::json::member<n_sync,   add, bool, &add::sync>,
        wfc::json::member<n_nores,  add, bool, &add::nores>,
        wfc::json::member<n_noval,  add, bool, &add::noval>,
        wfc::json::member<n_snapshot,  add, size_t, &add::snapshot>,
        wfc::json::member<n_update, add, add::field_list_t, &add::fields, raw_fields_list_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

namespace response
{
  struct add_json
  {
    JSON_NAME(prefix)
    JSON_NAME(status)
    JSON_NAME(fields)

    typedef wfc::json::object<
      add,
      wfc::json::member_list<
        wfc::json::member<n_prefix, add, std::string, &add::prefix>,
        wfc::json::member<n_fields, add, add::field_list_t, &add::fields, raw_fields_list_json>,
        wfc::json::member<n_status, add, common_status, &add::status, common_status_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}


