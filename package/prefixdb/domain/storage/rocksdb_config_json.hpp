#pragma once

#include <prefixdb/domain/storage/rocksdb_config.hpp>
#include <prefixdb/domain/storage/merge/merge_config_json.hpp>
#include <wfc/json.hpp>

namespace wamba{ namespace prefixdb{

struct slave_config_json
{
  JSON_NAME(enabled)
  JSON_NAME(target)
  JSON_NAME(start_time)
  JSON_NAME(pull_timeout_ms)
  JSON_NAME(log_limit_per_req)
  JSON_NAME(enable_progress)
  JSON_NAME(expires_for_req)
  JSON_NAME(acceptable_loss_seq)
  JSON_NAME(wrn_log_diff_seq)

  typedef ::wfc::json::object<
    slave_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_enabled,           slave_config, bool,        &slave_config::enabled>,
      ::wfc::json::member<n_target,            slave_config, std::string, &slave_config::target>,
      ::wfc::json::member<n_start_time,        slave_config, std::string, &slave_config::start_time>,
      ::wfc::json::member<n_pull_timeout_ms,   slave_config, time_t,      &slave_config::pull_timeout_ms>,
      ::wfc::json::member<n_log_limit_per_req, slave_config, size_t,      &slave_config::log_limit_per_req>,
      ::wfc::json::member<n_acceptable_loss_seq, slave_config, size_t,    &slave_config::acceptable_loss_seq>,
      ::wfc::json::member<n_wrn_log_diff_seq,  slave_config, size_t,       &slave_config::wrn_log_diff_seq>,
      ::wfc::json::member<n_enable_progress,   slave_config, bool,        &slave_config::enable_progress>,
      ::wfc::json::member<n_expires_for_req,   slave_config, bool,        &slave_config::expires_for_req>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

struct master_config_json
{
  typedef ::wfc::json::object<
    master_config,
    ::wfc::json::member_list<
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

  
struct rocksdb_config_json
{
  JSON_NAME(path)
  JSON_NAME(backup_path)
  JSON_NAME(restore_path)
  JSON_NAME(archive_path)
  JSON_NAME(ini)
  JSON_NAME(slave)
  JSON_NAME(master)
  JSON_NAME(compact_before_backup)

  typedef ::wfc::json::object<
    rocksdb_config,
    ::wfc::json::member_list<
      ::wfc::json::base<merge_config_json>,
      ::wfc::json::member<n_path,    rocksdb_config, std::string, &rocksdb_config::path>,
      ::wfc::json::member<n_backup_path,    rocksdb_config, std::string, &rocksdb_config::backup_path>,
      ::wfc::json::member<n_restore_path,    rocksdb_config, std::string, &rocksdb_config::restore_path>,
      ::wfc::json::member<n_archive_path,    rocksdb_config, std::string, &rocksdb_config::archive_path>,
      ::wfc::json::member<n_compact_before_backup, rocksdb_config, bool, &rocksdb_config::compact_before_backup>,
      ::wfc::json::member<n_ini,     rocksdb_config, std::string, &rocksdb_config::ini>,
      ::wfc::json::member<n_slave,     rocksdb_config, slave_config, &rocksdb_config::slave, slave_config_json>,
      ::wfc::json::member<n_master,     rocksdb_config, master_config, &rocksdb_config::master, master_config_json>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}

