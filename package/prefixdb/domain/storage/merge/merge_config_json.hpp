#pragma once

#include "merge_config.hpp"
#include <wfc/json.hpp>

namespace wamba{ namespace prefixdb{

struct merge_config_json
{
  JSON_NAME(packed_limit)
  JSON_NAME(array_limit)
  JSON_NAME(json_limit)

  typedef ::wfc::json::object<
    merge_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_packed_limit, merge_config, size_t, &merge_config::packed_limit>,
      ::wfc::json::member<n_array_limit, merge_config, size_t, &merge_config::array_limit>,
      ::wfc::json::member<n_json_limit, merge_config, size_t, &merge_config::json_limit>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}

