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

    typedef wfc::json::object<
      del,
      wfc::json::member_list<
        wfc::json::member<n_sync,   del, bool, &del::sync>,
        wfc::json::member<n_nores,  del, bool, &del::nores>,
        wfc::json::member<n_noval,  del, bool, &del::noval>,
        wfc::json::member<n_snapshot,  del, size_t, &del::snapshot>,
        wfc::json::member<n_prefix, del, std::string, &del::prefix>,
        wfc::json::member<n_fields, del, del::field_list_t, &del::fields, key_list_json >
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };

}

namespace response
{
  struct del_json
  {
    JSON_NAME(prefix)
    JSON_NAME(status)
    JSON_NAME(fields)

    /*
    typedef wfc::json::object<
      del::field,
      wfc::json::member_list<
        wfc::json::base< basic_field_json >
      >
    > field_json;
    typedef wfc::json::array< std::vector< field_json > > array_of_fields_json;
    */

    typedef wfc::json::object<
      del,
      wfc::json::member_list<
        wfc::json::member<n_prefix, del, std::string, &del::prefix>,
        wfc::json::member<n_status, del, common_status, &del::status, common_status_json>,
        wfc::json::member<n_fields, del, del::field_list_t, &del::fields, raw_fields_list_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
