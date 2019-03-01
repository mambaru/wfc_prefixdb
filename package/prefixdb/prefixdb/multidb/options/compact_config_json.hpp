#pragma once

#include <prefixdb/prefixdb/multidb/options/compact_config.hpp>
#include <wfc/json.hpp>

namespace wamba{ namespace prefixdb{

struct compact_config_json
{
  JSON_NAME(enabled)
  JSON_NAME(startup_compact)
  JSON_NAME(start_time)
  JSON_NAME(period_s)

  typedef ::wfc::json::object<
    compact_config,
    wfc::json::member_list<
      wfc::json::member<n_enabled,       compact_config, bool,        &compact_config::enabled>,
      wfc::json::member<n_startup_compact,       compact_config, bool,        &compact_config::startup_compact>,
      wfc::json::member<n_start_time,    compact_config, std::string, &compact_config::start_time>,
      wfc::json::member<n_period_s,      compact_config, time_t,      &compact_config::period_s>
    >,
    wfc::json::strict_mode
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}

