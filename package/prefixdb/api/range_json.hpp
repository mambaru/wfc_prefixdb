#pragma once

#include <prefixdb/api/range.hpp>
#include <prefixdb/api/fields_json.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/json.hpp>
#include <wfc/statistics/api/aggregated_json.hpp>

namespace wamba { namespace prefixdb {

namespace request
{
  struct range_json
  {
    JSON_NAME(prefix)
    JSON_NAME(nores)
    JSON_NAME(noval)
    JSON_NAME(stat)
    JSON_NAME(snapshot)
    JSON_NAME(beg)
    JSON_NAME(repair_json)
    JSON_NAME(from)
    JSON_NAME(to)
    JSON_NAME(limit)
    JSON_NAME(offset)

    typedef wfc::json::object<
      range,
      wfc::json::member_list<
        wfc::json::member<n_prefix, range, std::string, &range::prefix>,
        wfc::json::member<n_nores, range, bool, &range::nores>,
        wfc::json::member<n_noval, range, bool, &range::noval>,
        wfc::json::member<n_stat, range, bool, &range::stat>,
        wfc::json::member<n_snapshot,  range, size_t, &range::snapshot>,
        wfc::json::member<n_beg, range, bool, &range::beg>,
        wfc::json::member<n_repair_json, range, bool, &range::repair_json>,
        wfc::json::member<n_from, range, std::string, &range::from>,
        wfc::json::member<n_to, range, std::string, &range::to>,
        wfc::json::member<n_limit, range, size_t, &range::limit>,
        wfc::json::member<n_offset, range, size_t, &range::offset>
      >
    > type;

    typedef type::target target;
    typedef type::serializer serializer;
  };
}

namespace response
{
  struct range_json
  {
    struct stat_json
    {
      JSON_NAME(null_count)
      JSON_NAME(bool_count)
      JSON_NAME(number_count)
      JSON_NAME(string_count)
      JSON_NAME(array_count)
      JSON_NAME(object_count)
      JSON_NAME(repair_count)
      JSON_NAME(empty_count)
      JSON_NAME(keys)
      JSON_NAME(values)

      typedef wfc::json::object<
        range::stat_info,
        wfc::json::member_list<
          wfc::json::member<n_null_count,  range::stat_info, size_t, &range::stat_info::null_count>,
          wfc::json::member<n_bool_count,  range::stat_info, size_t, &range::stat_info::bool_count>,
          wfc::json::member<n_number_count,  range::stat_info, size_t, &range::stat_info::number_count>,
          wfc::json::member<n_string_count,  range::stat_info, size_t, &range::stat_info::string_count>,
          wfc::json::member<n_array_count,  range::stat_info, size_t, &range::stat_info::array_count>,
          wfc::json::member<n_object_count,  range::stat_info, size_t, &range::stat_info::object_count>,
          wfc::json::member<n_repair_count,  range::stat_info, size_t, &range::stat_info::repair_count>,
          wfc::json::member<n_empty_count,  range::stat_info, size_t, &range::stat_info::empty_count>,
          wfc::json::member<n_keys,  range::stat_info, wrtstat::aggregated_info, &range::stat_info::keys, wfc::statistics::aggregated_info_json>,
          wfc::json::member<n_values,  range::stat_info, wrtstat::aggregated_info, &range::stat_info::values, wfc::statistics::aggregated_info_json>
        >
      > type;
      typedef type::target target;
      typedef type::serializer serializer;
      typedef type::member_list member_list;
    };


    JSON_NAME(prefix)
    JSON_NAME2(n_final, "final")
    JSON_NAME(status)
    JSON_NAME(fields)
    JSON_NAME(stat)

    typedef wfc::json::object<
      range,
      wfc::json::member_list<
        wfc::json::member<n_final,  range, bool, &range::fin>,
        wfc::json::member<n_prefix, range, std::string, &range::prefix>,
        wfc::json::member<n_status, range, common_status, &range::status, common_status_json>,
        wfc::json::member<n_fields, range, field_list_t, &range::fields, fields_list_json>,
        wfc::json::member<n_stat, range, range::stat_info::ptr, &range::stat, wfc::json::pointer<range::stat_info::ptr, stat_json> >
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
