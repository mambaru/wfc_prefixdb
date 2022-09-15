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
    typedef wjson::object<
      has,
      wjson::member_list<
        wjson::member<n_snapshot,  has, size_t, &has::snapshot>,
        wjson::member<n_prefix, has, std::string, &has::prefix>,
        wjson::member<n_fields, has, key_list_t, &has::fields, key_list_json >
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };

}

namespace response
{
  struct has_json
  {
    JSON_NAME(prefix)
    JSON_NAME(status)
    JSON_NAME(fields)

    typedef wjson::object<
      has,
      wjson::member_list<
        wjson::member<n_prefix, has, std::string, &has::prefix>,
        wjson::member<n_status, has, common_status, &has::status, common_status_json>,
        wjson::member<n_fields, has, field_list_t, &has::fields, fields_list_json>
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

}}
