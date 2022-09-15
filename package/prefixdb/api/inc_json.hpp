#pragma once

#include <prefixdb/api/inc.hpp>
#include <prefixdb/api/add.hpp>
#include <prefixdb/api/fields_json.hpp>
#include <prefixdb/api/common_status_json.hpp>
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
    JSON_NAME(snapshot)

    typedef wjson::object<
      inc,
      wjson::member_list<
        wjson::member<n_prefix, inc, std::string, &inc::prefix>,
        wjson::member<n_sync,   inc, bool, &inc::sync>,
        wjson::member<n_nores,  inc, bool, &inc::nores>,
        wjson::member<n_snapshot,  inc, size_t, &inc::snapshot>,
        wjson::member<n_update, inc, field_list_t, &inc::fields, fields_list_json>
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

namespace response
{
  struct inc_json
  {
    JSON_NAME(prefix)
    JSON_NAME(status)
    JSON_NAME(fields)

    typedef wjson::object<
      inc,
      wjson::member_list<
        wjson::member<n_prefix, inc, std::string, &inc::prefix>,
        wjson::member<n_status, inc, common_status, &inc::status, common_status_json>,
        wjson::member<n_fields, inc, field_list_t, &inc::fields, fields_list_json>
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

}}
