#pragma once

#include <prefixdb/api/range.hpp>
#include <prefixdb/api/fields_json.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wrtstat/aggregator/api/json/aggregated_info_json.hpp>
#include <wfc/json.hpp>

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

    typedef wjson::object<
      range,
      wjson::member_list<
        wjson::member<n_prefix, range, std::string, &range::prefix>,
        wjson::member<n_nores, range, bool, &range::nores>,
        wjson::member<n_noval, range, bool, &range::noval>,
        wjson::member<n_stat, range, bool, &range::stat>,
        wjson::member<n_snapshot,  range, size_t, &range::snapshot>,
        wjson::member<n_beg, range, bool, &range::beg>,
        wjson::member<n_repair_json, range, bool, &range::repair_json>,
        wjson::member<n_from, range, std::string, &range::from>,
        wjson::member<n_to, range, std::string, &range::to>,
        wjson::member<n_limit, range, size_t, &range::limit>,
        wjson::member<n_offset, range, size_t, &range::offset>
      >
    > meta;

    typedef meta::target target;
    typedef meta::serializer serializer;
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

      typedef wjson::object<
        range::stat_info,
        wjson::member_list<
          wjson::member<n_null_count,  range::stat_info, size_t, &range::stat_info::null_count>,
          wjson::member<n_bool_count,  range::stat_info, size_t, &range::stat_info::bool_count>,
          wjson::member<n_number_count,  range::stat_info, size_t, &range::stat_info::number_count>,
          wjson::member<n_string_count,  range::stat_info, size_t, &range::stat_info::string_count>,
          wjson::member<n_array_count,  range::stat_info, size_t, &range::stat_info::array_count>,
          wjson::member<n_object_count,  range::stat_info, size_t, &range::stat_info::object_count>,
          wjson::member<n_repair_count,  range::stat_info, size_t, &range::stat_info::repair_count>,
          wjson::member<n_empty_count,  range::stat_info, size_t, &range::stat_info::empty_count>,
          wjson::member<n_keys,  range::stat_info, wrtstat::aggregated_info, &range::stat_info::keys, 
                            wrtstat::aggregated_info_json>,
          wjson::member<n_values,  range::stat_info, wrtstat::aggregated_info, &range::stat_info::values, 
                            wrtstat::aggregated_info_json>
        >
      > meta;
      typedef meta::target target;
      typedef meta::serializer serializer;
      typedef meta::member_list member_list;
    };


    JSON_NAME(prefix)
    JSON_NAME2(n_final, "final")
    JSON_NAME(status)
    JSON_NAME(fields)
    JSON_NAME(stat)

    typedef wjson::object<
      range,
      wjson::member_list<
        wjson::member<n_final,  range, bool, &range::fin>,
        wjson::member<n_prefix, range, std::string, &range::prefix>,
        wjson::member<n_status, range, common_status, &range::status, common_status_json>,
        wjson::member<n_fields, range, field_list_t, &range::fields, fields_list_json>,
        wjson::member<n_stat, range, range::stat_info::ptr, &range::stat, wjson::pointer<range::stat_info::ptr, stat_json> >
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

}}
