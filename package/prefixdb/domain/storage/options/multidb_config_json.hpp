#pragma once

#include <prefixdb/domain/storage/options/multidb_config.hpp>
#include <prefixdb/domain/storage/options/db_config_json.hpp>
#include <wfc/json.hpp>

namespace wamba{ namespace prefixdb{

struct multidb_config_json
{
  // Предварительное открытие всех баз префиксов
  JSON_NAME(preopen)
  JSON_NAME(keys_per_req)
  JSON_NAME(key_size_limit)
  JSON_NAME(value_size_limit)
  JSON_NAME(prefix_size_limit)
  JSON_NAME(max_prefixes)
  JSON_NAME(workflow)

  typedef ::wfc::json::object<
    multidb_config,
    ::wfc::json::member_list<
      ::wfc::json::base<db_config_json>,
      ::wfc::json::member<n_preopen, multidb_config, bool,        &multidb_config::preopen>,
      ::wfc::json::member<n_keys_per_req, multidb_config, size_t, &multidb_config::keys_per_req>,
      ::wfc::json::member<n_key_size_limit, multidb_config, size_t, &multidb_config::key_size_limit>,
      ::wfc::json::member<n_value_size_limit, multidb_config, size_t, &multidb_config::value_size_limit>,
      ::wfc::json::member<n_prefix_size_limit, multidb_config, size_t, &multidb_config::prefix_size_limit>,
      ::wfc::json::member<n_max_prefixes, multidb_config, size_t, &multidb_config::max_prefixes>,
      ::wfc::json::member<n_workflow, multidb_config, ::wfc::workflow_options, &multidb_config::workflow, ::wfc::workflow_options_json>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}

