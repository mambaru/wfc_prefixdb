#pragma once

#include <prefixdb/api/setnx.hpp>
#include <prefixdb/api/fields_json.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct setnx_json
  {
    JSON_NAME(prefix)
    JSON_NAME(nores)
    JSON_NAME(noval)
    JSON_NAME(snapshot)
    JSON_NAME(fields)
    JSON_NAME(sync)

    typedef wjson::object<
      setnx,
      wjson::member_list<
        wjson::member<n_prefix, setnx, std::string, &setnx::prefix>,
        wjson::member<n_sync,   setnx, bool, &setnx::sync>,
        wjson::member<n_nores,  setnx, bool, &setnx::nores>,
        wjson::member<n_noval,  setnx, bool, &setnx::noval>,
        wjson::member<n_snapshot, setnx, size_t, &setnx::snapshot>,
        wjson::member<n_fields, setnx, field_list_t, &setnx::fields, fields_list_json >
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

namespace response
{
  struct setnx_json
  {
    JSON_NAME(prefix)
    JSON_NAME(status)
    JSON_NAME(fields)

    typedef wjson::object<
      setnx,
      wjson::member_list<
        wjson::member<n_prefix, setnx, std::string,       &setnx::prefix>,
        wjson::member<n_status, setnx, common_status,     &setnx::status, common_status_json>,
        wjson::member<n_fields, setnx, field_list_t, &setnx::fields, fields_list_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
