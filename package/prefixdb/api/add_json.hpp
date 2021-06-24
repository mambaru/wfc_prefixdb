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
    
    typedef wjson::object<
      add,
      wjson::member_list<
        wjson::member<n_prefix, add, std::string, &add::prefix>,
        wjson::member<n_sync,   add, bool, &add::sync>,
        wjson::member<n_nores,  add, bool, &add::nores>,
        wjson::member<n_noval,  add, bool, &add::noval>,
        wjson::member<n_snapshot,  add, size_t, &add::snapshot>,
        wjson::member<n_update, add, field_list_t, &add::fields, fields_list_json>
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

    typedef wjson::object<
      add,
      wjson::member_list<
        wjson::member<n_prefix, add, std::string, &add::prefix>,
        wjson::member<n_fields, add, field_list_t, &add::fields, fields_list_json>,
        wjson::member<n_status, add, common_status, &add::status, common_status_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}


