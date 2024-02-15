#pragma once

#include <prefixdb/domain/multidb/options/compact_config.hpp>
#include <wjson/wjson.hpp>

namespace wamba{ namespace prefixdb{

struct compact_config_json
{
  JSON_NAME(enabled)
  JSON_NAME(startup_compact)
  JSON_NAME(start_time)
  JSON_NAME(period_s)

  typedef wjson::object<
    compact_config,
    wjson::member_list<
      wjson::member<n_enabled,         compact_config, bool,        &compact_config::enabled>,
      wjson::member<n_startup_compact, compact_config, bool,        &compact_config::startup_compact>,
      wjson::member<n_start_time,      compact_config, std::string, &compact_config::start_time>,
      wjson::member<n_period_s,        compact_config, time_t,      &compact_config::period_s>
    >,
    wjson::strict_mode
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}

