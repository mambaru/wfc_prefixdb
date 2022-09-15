#pragma once

#include <prefixdb/api/get.hpp>
#include <prefixdb/api/add.hpp>
#include <prefixdb/api/fields_json.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct get_json
  {
    JSON_NAME(prefix)
    JSON_NAME(fields)
    JSON_NAME(snapshot)
    typedef wjson::object<
      get,
      wjson::member_list<
        wjson::member<n_snapshot,  get, size_t, &get::snapshot>,
        wjson::member<n_prefix, get, std::string, &get::prefix>,
        wjson::member<n_fields, get, key_list_t, &get::fields, key_list_json >
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };

}

namespace response
{
  struct get_json
  {
    JSON_NAME(prefix)
    JSON_NAME(status)
    JSON_NAME(fields)

    typedef wjson::object<
      get,
      wjson::member_list<
        wjson::member<n_prefix, get, std::string, &get::prefix>,
        wjson::member<n_status, get, common_status, &get::status, common_status_json>,
        wjson::member<n_fields, get, field_list_t, &get::fields, fields_list_json>
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

}}
