#pragma once

#include <storage/api/set.hpp>
#include <wfc/json.hpp>
#include "aux_json.hpp"

namespace wamba { namespace leveldb {

namespace request 
{
  struct set_json
  {
    typedef wfc::json::member_value
    <
      set,
      set,
      kv_array_t,
      &set::kv_array,
      kv_array_json_t
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
  };

}

namespace response
{
  struct set_json
  {
    typedef wfc::json::member_value
    <
      set,
      set,
      bool,
      &set::result,
      wfc::json::value<bool>
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
  };

  //typedef wfc::json::value<bool> set_json;
}

}}
