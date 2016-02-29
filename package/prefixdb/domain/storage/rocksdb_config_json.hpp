#pragma once

#include <prefixdb/domain/storage/rocksdb_config.hpp>
#include <prefixdb/domain/storage/merge/merge_config_json.hpp>
#include <wfc/json.hpp>

namespace wamba{ namespace prefixdb{

struct rocksdb_config_json
{
  // Путь к базам rocksdb (должен существовать)
  JSON_NAME(path)
  JSON_NAME(backup_path)
  JSON_NAME(restore_path)
  // Файл опций в формате ini
  JSON_NAME(ini)

  typedef ::wfc::json::object<
    rocksdb_config,
    ::wfc::json::member_list<
      ::wfc::json::base<merge_config_json>,
      ::wfc::json::member<n_path,    rocksdb_config, std::string, &rocksdb_config::path>,
      ::wfc::json::member<n_backup_path,    rocksdb_config, std::string, &rocksdb_config::backup_path>,
      ::wfc::json::member<n_restore_path,    rocksdb_config, std::string, &rocksdb_config::restore_path>,
      ::wfc::json::member<n_ini,     rocksdb_config, std::string, &rocksdb_config::ini>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}

