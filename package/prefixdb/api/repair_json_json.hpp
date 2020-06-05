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

    typedef wfc::json::object<
      repair_json,
      wfc::json::member_list<
        wfc::json::member<n_prefix, repair_json, std::string, &repair_json::prefix>,
        wfc::json::member<n_nores, repair_json, bool, &repair_json::nores>,
        wfc::json::member<n_noval, repair_json, bool, &repair_json::noval>,
        wfc::json::member<n_sync, repair_json, bool, &repair_json::sync>,
        wfc::json::member<n_snapshot,  repair_json, size_t, &repair_json::snapshot>,
        wfc::json::member<n_beg, repair_json, bool, &repair_json::beg>,
        wfc::json::member<n_from, repair_json, std::string, &repair_json::from>,
        wfc::json::member<n_to, repair_json, std::string, &repair_json::to>,
        wfc::json::member<n_limit, repair_json, size_t, &repair_json::limit>,
        wfc::json::member<n_offset, repair_json, size_t, &repair_json::offset>
      >
    > type;

    typedef type::target target;
    typedef type::serializer serializer;
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


    typedef wfc::json::object<
      repair_json,
      wfc::json::member_list<
        wfc::json::member<n_prefix, repair_json, std::string, &repair_json::prefix>,
        wfc::json::member<n_status, repair_json, common_status, &repair_json::status, common_status_json>,
        wfc::json::member<n_final,  repair_json, bool, &repair_json::fin>,
        wfc::json::member<n_total,  repair_json, size_t, &repair_json::total>,
        wfc::json::member<n_repaired,  repair_json, size_t, &repair_json::repaired>,
        wfc::json::member<n_last_key, repair_json, std::string, &repair_json::last_key>,
        wfc::json::member<n_fields, repair_json, field_list_t, &repair_json::fields, fields_list_json>

      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
