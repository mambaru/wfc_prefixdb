#pragma once

#include <prefixdb/api/packed.hpp>
#include <prefixdb/api/add.hpp>
#include <prefixdb/api/fields_json.hpp>
#include <prefixdb/api/common_status_json.hpp>
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
    JSON_NAME(snapshot)
    
    typedef wjson::object<
      packed,
      wjson::member_list<
        wjson::member<n_prefix, packed, std::string, &packed::prefix>,
        wjson::member<n_sync,   packed, bool, &packed::sync>,
        wjson::member<n_nores,  packed, bool, &packed::nores>,
        wjson::member<n_snapshot,  packed, size_t, &packed::snapshot>,
        wjson::member<n_update, packed, field_list_t, &packed::fields, fields_list_json>
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

namespace response
{
  struct packed_json
  {
    JSON_NAME(prefix)
    JSON_NAME(status)
    JSON_NAME(fields)

    typedef wjson::object<
      packed,
      wjson::member_list<
        wjson::member<n_prefix, packed, std::string, &packed::prefix>,
        wjson::member<n_fields, packed, field_list_t, &packed::fields, fields_list_json>,
        wjson::member<n_status, packed, common_status, &packed::status, common_status_json>
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

}}
