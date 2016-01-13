#pragma once

#include <wfc/json.hpp>
#include <wfc/name.hpp>
#include <prefixdb/domain/prefixdb_config.hpp>

namespace wamba{ namespace prefixdb{

struct prefixdb_config_json
{
  typedef wfc::json::object<
    prefixdb_config,
    ::wfc::json::member_list<
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list; 
};

}}
