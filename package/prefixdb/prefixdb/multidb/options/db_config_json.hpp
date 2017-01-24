#pragma once

#include <prefixdb/prefixdb/multidb/options/db_config.hpp>
#include <prefixdb/prefixdb/multidb/options/slave_config_json.hpp>
#include <prefixdb/prefixdb/multidb/options/backup_config_json.hpp>
#include <prefixdb/prefixdb/multidb/options/archive_config_json.hpp>
#include <prefixdb/prefixdb/multidb/options/restore_config_json.hpp>
#include <wfc/json.hpp>

namespace wamba{ namespace prefixdb{

  
struct db_config_json
{
  JSON_NAME(path)
  JSON_NAME(detach_path)
  JSON_NAME(ini)
  JSON_NAME(slave)
  //JSON_NAME(master)
  JSON_NAME(packed_limit)
  JSON_NAME(array_limit)
  JSON_NAME(range_limit)
  JSON_NAME(backup)
  JSON_NAME(archive)
  JSON_NAME(restore)
  JSON_NAME(workflow)
  JSON_NAME(enable_delayed_write)
  JSON_NAME(auto_repair)
  JSON_NAME(abort_if_open_error)
  JSON_NAME(check_merge_operations)

  typedef ::wfc::json::object<
    db_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_path,    db_config, std::string, &db_config::path>,
      ::wfc::json::member<n_detach_path,    db_config, std::string, &db_config::detach_path>,
      ::wfc::json::member<n_packed_limit, db_config, size_t, &db_config::packed_limit>,
      ::wfc::json::member<n_array_limit, db_config, size_t, &db_config::array_limit>,
      ::wfc::json::member<n_range_limit, db_config, size_t, &db_config::range_limit>,
      ::wfc::json::member<n_enable_delayed_write, db_config, bool, &db_config::enable_delayed_write>,
      ::wfc::json::member<n_auto_repair, db_config, bool, &db_config::auto_repair>,
      ::wfc::json::member<n_abort_if_open_error, db_config, bool, &db_config::abort_if_open_error>,
      ::wfc::json::member<n_check_merge_operations, db_config, bool, &db_config::abort_if_open_error>,
      ::wfc::json::member<n_ini,          db_config, std::string,    &db_config::ini>,
      ::wfc::json::member<n_slave,        db_config, slave_config,   &db_config::slave, slave_config_json>,
      ::wfc::json::member<n_backup,       db_config, backup_config,  &db_config::backup, backup_config_json>,
      ::wfc::json::member<n_archive,      db_config, archive_config, &db_config::archive, archive_config_json>,
      ::wfc::json::member<n_restore,      db_config, restore_config, &db_config::restore, restore_config_json>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}

