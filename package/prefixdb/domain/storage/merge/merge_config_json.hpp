#pragma once

#include "merge_config.hpp"
#include <wfc/json.hpp>

namespace wamba{ namespace prefixdb{

struct merge_config_json
{
  typedef ::wfc::json::object<
    merge_config,
    ::wfc::json::member_list<
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
