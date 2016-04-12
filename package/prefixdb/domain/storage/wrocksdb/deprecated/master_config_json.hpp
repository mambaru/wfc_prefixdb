#pragma once

#include <prefixdb/domain/storage/options/master_config.hpp>
#include <wfc/json.hpp>

namespace wamba{ namespace prefixdb{

struct master_config_json
{
  JSON_NAME(enabled)
  JSON_NAME(log_buffer_size)

  typedef ::wfc::json::object<
    master_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_enabled,         master_config, bool,        &master_config::enabled>,
      ::wfc::json::member<n_log_buffer_size, master_config, size_t,      &master_config::log_buffer_size>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};


}}

