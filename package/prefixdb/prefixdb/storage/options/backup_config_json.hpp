#pragma once

#include <prefixdb/prefixdb/storage/options/backup_config.hpp>
#include <wfc/json.hpp>

namespace wamba{ namespace prefixdb{

struct backup_config_json
{
  JSON_NAME(enabled)
  JSON_NAME(path)
  JSON_NAME(depth)
  JSON_NAME(start_time)
  JSON_NAME(period_s)
  JSON_NAME(start_delay_s)

  typedef ::wfc::json::object<
    backup_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_enabled,       backup_config, bool,        &backup_config::enabled>,
      ::wfc::json::member<n_period_s,      backup_config, time_t,      &backup_config::period_s>,
      ::wfc::json::member<n_start_delay_s, backup_config, time_t,      &backup_config::start_delay_s>,
      ::wfc::json::member<n_start_time,    backup_config, std::string, &backup_config::start_time>,
      ::wfc::json::member<n_depth,      backup_config, size_t,      &backup_config::depth>,
      ::wfc::json::member<n_path,          backup_config, std::string, &backup_config::path>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}

