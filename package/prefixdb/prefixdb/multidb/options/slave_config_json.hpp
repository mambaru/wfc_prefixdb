#pragma once

#include <prefixdb/prefixdb/multidb/options/slave_config.hpp>
#include <wfc/json.hpp>

namespace wamba{ namespace prefixdb{

struct slave_config_json
{
  JSON_NAME(enabled)
  JSON_NAME(target)
  JSON_NAME(start_time)
  JSON_NAME(pull_timeout_ms)
  JSON_NAME(log_limit_per_req)
  JSON_NAME(expires_for_req)
  JSON_NAME(acceptable_loss_seq)
  JSON_NAME(wrn_log_diff_seq)
  JSON_NAME(wrn_log_timeout_ms)
  JSON_NAME(seq_log_timeout_ms)
  JSON_NAME(query_prefixes_timeout_ms)
  JSON_NAME(disableWAL)

  typedef wjson::object<
    slave_config,
    wjson::member_list<
      wjson::member<n_enabled,           slave_config, bool,        &slave_config::enabled>,
      wjson::member<n_target,            slave_config, std::string, &slave_config::target>,
      wjson::member<n_start_time,        slave_config, std::string, &slave_config::start_time>,
      wjson::member<n_pull_timeout_ms,   slave_config, time_t,      &slave_config::pull_timeout_ms>,
      wjson::member<n_query_prefixes_timeout_ms, slave_config, time_t,      &slave_config::query_prefixes_timeout_ms>,
      wjson::member<n_log_limit_per_req, slave_config, size_t,      &slave_config::log_limit_per_req>,
      wjson::member<n_acceptable_loss_seq, slave_config, std::ptrdiff_t,    &slave_config::acceptable_loss_seq>,
      wjson::member<n_wrn_log_diff_seq,  slave_config, std::ptrdiff_t,       &slave_config::wrn_log_diff_seq>,
      wjson::member<n_wrn_log_timeout_ms,  slave_config, time_t,       &slave_config::wrn_log_timeout_ms>,
      wjson::member<n_seq_log_timeout_ms,  slave_config, time_t,       &slave_config::seq_log_timeout_ms>,
      wjson::member<n_expires_for_req,   slave_config, bool,        &slave_config::expires_for_req>,
      wjson::member<n_disableWAL,  slave_config, bool,       &slave_config::disableWAL>
    >,
    wjson::strict_mode
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};


}}

