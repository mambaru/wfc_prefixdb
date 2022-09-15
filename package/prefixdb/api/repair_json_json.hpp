#pragma once

#include <prefixdb/api/repair_json.hpp>
#include <prefixdb/api/fields_json.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request
{
  struct repair_json_json
  {
    JSON_NAME(prefix)
    JSON_NAME(nores)
    JSON_NAME(noval)
    JSON_NAME(sync)
    JSON_NAME(snapshot)
    JSON_NAME(beg)
    JSON_NAME(from)
    JSON_NAME(to)
    JSON_NAME(limit)
    JSON_NAME(offset)

    typedef wjson::object<
      repair_json,
      wjson::member_list<
        wjson::member<n_prefix, repair_json, std::string, &repair_json::prefix>,
        wjson::member<n_nores, repair_json, bool, &repair_json::nores>,
        wjson::member<n_noval, repair_json, bool, &repair_json::noval>,
        wjson::member<n_sync, repair_json, bool, &repair_json::sync>,
        wjson::member<n_snapshot,  repair_json, size_t, &repair_json::snapshot>,
        wjson::member<n_beg, repair_json, bool, &repair_json::beg>,
        wjson::member<n_from, repair_json, std::string, &repair_json::from>,
        wjson::member<n_to, repair_json, std::string, &repair_json::to>,
        wjson::member<n_limit, repair_json, size_t, &repair_json::limit>,
        wjson::member<n_offset, repair_json, size_t, &repair_json::offset>
      >
    > meta;

    typedef meta::target target;
    typedef meta::serializer serializer;
  };
}

namespace response
{
  struct repair_json_json
  {
    JSON_NAME(prefix)
    JSON_NAME2(n_final, "final")
    JSON_NAME(status)
    JSON_NAME(fields)
    JSON_NAME(last_key)
    JSON_NAME(total)
    JSON_NAME(repaired)


    typedef wjson::object<
      repair_json,
      wjson::member_list<
        wjson::member<n_prefix, repair_json, std::string, &repair_json::prefix>,
        wjson::member<n_status, repair_json, common_status, &repair_json::status, common_status_json>,
        wjson::member<n_final,  repair_json, bool, &repair_json::fin>,
        wjson::member<n_total,  repair_json, size_t, &repair_json::total>,
        wjson::member<n_repaired,  repair_json, size_t, &repair_json::repaired>,
        wjson::member<n_last_key, repair_json, std::string, &repair_json::last_key>,
        wjson::member<n_fields, repair_json, field_list_t, &repair_json::fields, fields_list_json>

      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

}}
