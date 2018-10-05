#pragma once

#include <prefixdb/prefixdb/multidb/options/restore_config.hpp>
#include <wfc/json.hpp>

namespace wamba{ namespace prefixdb{


struct restore_config_json
{
  JSON_NAME(forbid)
  JSON_NAME(path)
  JSON_NAME(backup_id)
  
  typedef wfc::json::object<
    restore_config,
    wfc::json::member_list<
      wfc::json::member<n_forbid,    restore_config, bool,        &restore_config::forbid>,
      wfc::json::member<n_backup_id, restore_config, int64_t,     &restore_config::backup_id>,
      wfc::json::member<n_path,      restore_config, std::string, &restore_config::path>
    >,
    wfc::json::strict_mode
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};


}}

