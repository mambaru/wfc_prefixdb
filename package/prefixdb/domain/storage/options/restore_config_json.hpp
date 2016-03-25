#pragma once

#include <prefixdb/domain/storage/options/restore_config.hpp>
#include <wfc/json.hpp>

namespace wamba{ namespace prefixdb{


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


}}

