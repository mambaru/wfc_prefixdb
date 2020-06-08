#pragma once

#include <prefixdb/prefixdb/multidb/options/backup_config.hpp>
#include <wjson/wjson.hpp>

namespace wamba{ namespace prefixdb{

struct backup_config_json
{
  JSON_NAME(enabled)
  JSON_NAME(path)
  JSON_NAME(depth)
  JSON_NAME(start_time)
  JSON_NAME(period_s)

  typedef wjson::object<
    backup_config,
    wjson::member_list<
      wjson::member<n_enabled,       backup_config, bool,        &backup_config::enabled>,
      wjson::member<n_period_s,      backup_config, time_t,      &backup_config::period_s>,
      wjson::member<n_start_time,    backup_config, std::string, &backup_config::start_time>,
      wjson::member<n_depth,         backup_config, uint32_t,      &backup_config::depth>,
      wjson::member<n_path,          backup_config, std::string, &backup_config::path>
    >,
    wjson::strict_mode
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}

