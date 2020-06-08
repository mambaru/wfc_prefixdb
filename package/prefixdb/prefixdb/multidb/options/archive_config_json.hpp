#pragma once

#include <prefixdb/prefixdb/multidb/options/archive_config.hpp>
#include <wjson/wjson.hpp>

namespace wamba{ namespace prefixdb{

struct archive_config_json
{
  JSON_NAME(enabled)
  JSON_NAME(path)
  JSON_NAME(start_time)
  JSON_NAME(period_s)
  JSON_NAME(depth)

  typedef wjson::object<
    archive_config,
    wjson::member_list<
      wjson::member<n_enabled,       archive_config, bool,        &archive_config::enabled>,
      wjson::member<n_period_s,      archive_config, time_t,      &archive_config::period_s>,
      wjson::member<n_start_time,    archive_config, std::string, &archive_config::start_time>,
      wjson::member<n_path,          archive_config, std::string, &archive_config::path>,
      wjson::member<n_depth,         archive_config, size_t,      &archive_config::depth>
    >,
    wjson::strict_mode
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}

