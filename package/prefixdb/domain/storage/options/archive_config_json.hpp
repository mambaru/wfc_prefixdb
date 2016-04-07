#pragma once

#include <prefixdb/domain/storage/options/archive_config.hpp>
#include <wfc/json.hpp>

namespace wamba{ namespace prefixdb{

struct archive_config_json
{
  JSON_NAME(enabled)
  JSON_NAME(path)
  JSON_NAME(start_time)
  JSON_NAME(period_s)
  JSON_NAME(depth)
  

  typedef ::wfc::json::object<
    archive_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_enabled,       archive_config, bool,        &archive_config::enabled>,
      ::wfc::json::member<n_period_s,      archive_config, time_t,      &archive_config::period_s>,
      ::wfc::json::member<n_start_time,    archive_config, std::string, &archive_config::start_time>,
      ::wfc::json::member<n_path,          archive_config, std::string, &archive_config::path>,
      ::wfc::json::member<n_depth,         archive_config, size_t,      &archive_config::depth>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}

