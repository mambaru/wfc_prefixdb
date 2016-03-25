#pragma once

#include <prefixdb/domain/storage/db_config.hpp>
#include <prefixdb/domain/storage/merge/merge_config_json.hpp>
#include <wfc/json.hpp>

namespace wamba{ namespace prefixdb{

struct backup_config_json
{
  JSON_NAME(enabled)
  JSON_NAME(path)
  JSON_NAME(start_time)
  JSON_NAME(period_s)
  JSON_NAME(start_delay_s)

  typedef ::wfc::json::object<
    backup_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_enabled,       backup_config, bool,        &backup_config::enabled>,
      ::wfc::json::member<n_period_s,      backup_config, time_t,      &backup_config::period_s>,
      ::wfc::json::member<n_start_delay_s, backup_config, time_t,      &backup_config::start_delay_s>,
      ::wfc::json::member<n_start_time,    backup_config, std::string, &backup_config::start_time>,
      ::wfc::json::member<n_path,          backup_config, std::string, &backup_config::path>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

struct archive_config_json
{
  JSON_NAME(enabled)
  JSON_NAME(path)
  JSON_NAME(start_time)
  JSON_NAME(period_s)
  

  typedef ::wfc::json::object<
    archive_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_enabled,       archive_config, bool,        &archive_config::enabled>,
      ::wfc::json::member<n_period_s,      archive_config, time_t,      &archive_config::period_s>,
      ::wfc::json::member<n_start_time,    archive_config, std::string, &archive_config::start_time>,
      ::wfc::json::member<n_path,          archive_config, std::string, &archive_config::path>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

struct restore_config_json
{
  JSON_NAME(forbid)
  JSON_NAME(path)
  

  typedef ::wfc::json::object<
    restore_config,
    ::wfc::json::member_list<
      ::wfc::json::member<n_forbid,       restore_config, bool,        &restore_config::forbid>,
      ::wfc::json::member<n_path,         restore_config, std::string, &restore_config::path>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

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
  JSON_NAME(seq_log_timeout_ms)
  

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
      ::wfc::json::member<n_seq_log_timeout_ms,  slave_config, size_t,       &slave_config::seq_log_timeout_ms>,
      ::wfc::json::member<n_enable_progress,   slave_config, bool,        &slave_config::enable_progress>,
      ::wfc::json::member<n_expires_for_req,   slave_config, bool,        &slave_config::expires_for_req>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

/*
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
};*/

  
struct db_config_json
{
  JSON_NAME(path)
  JSON_NAME(backup_path)
  JSON_NAME(restore_path)
  JSON_NAME(archive_path)
  JSON_NAME(ini)
  JSON_NAME(slave)
  JSON_NAME(master)
  JSON_NAME(compact_before_backup)
  JSON_NAME(packed_limit)
  JSON_NAME(array_limit)
  JSON_NAME(backup)
  JSON_NAME(archive)
  JSON_NAME(restore)

  typedef ::wfc::json::object<
    db_config,
    ::wfc::json::member_list<
      //::wfc::json::base<merge_config_json>,
      ::wfc::json::member<n_path,    db_config, std::string, &db_config::path>,
      ::wfc::json::member<n_packed_limit, db_config, size_t, &db_config::packed_limit>,
      ::wfc::json::member<n_array_limit, db_config, size_t, &db_config::array_limit>,

      ::wfc::json::member<n_restore_path, db_config, std::string,    &db_config::restore_path>,
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

