#pragma once

#include "multidb_config.hpp"
#include "merge/merge_config_json.hpp"
#include <wfc/json.hpp>

namespace wamba{ namespace prefixdb{

struct multidb_config_json
{
  // Предварительное открытие всех баз префиксов
  JSON_NAME(preopen)
  // Путь к базам rocksdb (должен существовать)
  JSON_NAME(path)
  // Файл опций в формате ini
  JSON_NAME(ini)

  typedef ::wfc::json::object<
    multidb_config,
    ::wfc::json::member_list<
      ::wfc::json::base<merge_config_json>,
      ::wfc::json::member<n_preopen, multidb_config, bool,        &multidb_config::preopen>,
      ::wfc::json::member<n_path,    multidb_config, std::string, &multidb_config::path>,
      ::wfc::json::member<n_ini,     multidb_config, std::string, &multidb_config::ini>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}

