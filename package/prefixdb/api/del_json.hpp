#pragma once

#include <prefixdb/api/del.hpp>
#include <prefixdb/api/fields_json.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/json.hpp>


namespace wamba { namespace prefixdb {

namespace request 
{
  struct del_json
  {
    JSON_NAME(prefix)
    JSON_NAME(fields)
    JSON_NAME(nores)
    JSON_NAME(noval)
    JSON_NAME(sync)
    JSON_NAME(snapshot)

    typedef wjson::object<
      del,
      wjson::member_list<
        wjson::member<n_sync,   del, bool, &del::sync>,
        wjson::member<n_nores,  del, bool, &del::nores>,
        wjson::member<n_noval,  del, bool, &del::noval>,
        wjson::member<n_snapshot,  del, size_t, &del::snapshot>,
        wjson::member<n_prefix, del, std::string, &del::prefix>,
        wjson::member<n_fields, del, key_list_t, &del::fields, key_list_json >
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };

}

namespace response
{
  struct del_json
  {
    JSON_NAME(prefix)
    JSON_NAME(status)
    JSON_NAME(fields)

    typedef wjson::object<
      del,
      wjson::member_list<
        wjson::member<n_prefix, del, std::string, &del::prefix>,
        wjson::member<n_status, del, common_status, &del::status, common_status_json>,
        wjson::member<n_fields, del, field_list_t, &del::fields, fields_list_json>
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

}}
