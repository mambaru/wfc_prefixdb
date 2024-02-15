#pragma once

#include <prefixdb/domain/multidb/options/multidb_config.hpp>
#include <prefixdb/domain/multidb/options/db_config_json.hpp>
#include <wjson/wjson.hpp>

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
  JSON_NAME(writable_prefixes)
  JSON_NAME(readonly_prefixes)

  typedef wjson::object<
    multidb_config,
    wjson::member_list<
      wjson::member<n_preopen, multidb_config, bool,        &multidb_config::preopen>,
      wjson::member<n_keys_per_req, multidb_config, size_t, &multidb_config::keys_per_req>,
      wjson::member<n_key_size_limit, multidb_config, size_t, &multidb_config::key_size_limit>,
      wjson::member<n_value_size_limit, multidb_config, size_t, &multidb_config::value_size_limit>,
      wjson::member<n_prefix_size_limit, multidb_config, size_t, &multidb_config::prefix_size_limit>,
      wjson::member<n_max_prefixes, multidb_config, size_t, &multidb_config::max_prefixes>,
      wjson::member<n_writable_prefixes, multidb_config, std::vector<std::string>, 
                                        &multidb_config::writable_prefixes, wjson::vector_of_strings<> >,
      wjson::member<n_readonly_prefixes, multidb_config, std::vector<std::string>, 
                                        &multidb_config::readonly_prefixes, wjson::vector_of_strings<> >,

      wjson::base<db_config_json>
    >,
    wjson::strict_mode
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}

