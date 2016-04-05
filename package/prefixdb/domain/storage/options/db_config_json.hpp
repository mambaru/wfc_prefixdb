#pragma once

#include <prefixdb/domain/storage/options/db_config.hpp>
#include <prefixdb/domain/storage/options/master_config_json.hpp>
#include <prefixdb/domain/storage/options/slave_config_json.hpp>
#include <prefixdb/domain/storage/options/backup_config_json.hpp>
#include <prefixdb/domain/storage/options/archive_config_json.hpp>
#include <prefixdb/domain/storage/options/restore_config_json.hpp>
#include <wfc/json.hpp>

namespace wamba{ namespace prefixdb{

  
struct db_config_json
{
  JSON_NAME(path)
  JSON_NAME(detach_path)
  JSON_NAME(ini)
  JSON_NAME(slave)
  JSON_NAME(master)
  JSON_NAME(packed_limit)
  JSON_NAME(array_limit)
  JSON_NAME(backup)
  JSON_NAME(archive)
  JSON_NAME(restore)
  JSON_NAME(workflow)

  typedef ::wfc::json::object<
    db_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_path,    db_config, std::string, &db_config::path>,
      ::wfc::json::member<n_detach_path,    db_config, std::string, &db_config::detach_path>,
      ::wfc::json::member<n_packed_limit, db_config, size_t, &db_config::packed_limit>,
      ::wfc::json::member<n_array_limit, db_config, size_t, &db_config::array_limit>,

      ::wfc::json::member<n_ini,          db_config, std::string,    &db_config::ini>,
      ::wfc::json::member<n_slave,        db_config, slave_config,   &db_config::slave, slave_config_json>,
      ::wfc::json::member<n_master,       db_config, master_config,  &db_config::master, master_config_json>,
      ::wfc::json::member<n_backup,       db_config, backup_config,  &db_config::backup, backup_config_json>,
      ::wfc::json::member<n_archive,      db_config, archive_config, &db_config::archive, archive_config_json>,
      ::wfc::json::member<n_restore,      db_config, restore_config, &db_config::restore, restore_config_json>,
      ::wfc::json::member<n_workflow, db_config, ::wfc::workflow_options, &db_config::workflow, ::wfc::workflow_options_json>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}

