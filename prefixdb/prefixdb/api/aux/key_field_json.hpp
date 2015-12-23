#pragma once

#include <prefixdb/api/aux/key_field.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

struct key_field_json
{
  JSON_NAME(key)

  typedef ::wfc::json::object<
    key_field,
    ::wfc::json::member_list<
        ::wfc::json::member<n_key, key_field, std::string, &key_field::key>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
  
};

}}
