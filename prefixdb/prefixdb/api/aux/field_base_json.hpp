#pragma once

#include <prefixdb/api/aux/field_base.hpp>
#include <prefixdb/api/aux/field_type_json.hpp>
#include <prefixdb/api/aux/key_field_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

struct field_base_json
{
  JSON_NAME(ttl)
  JSON_NAME(type)
  
  typedef ::wfc::json::object<
    field_base,
    ::wfc::json::member_list<
        ::wfc::json::base<key_field_json>,
        ::wfc::json::member<n_ttl, field_base, time_t, &field_base::ttl>,
        ::wfc::json::member<n_type, field_base, field_type, &field_base::type, field_type_json>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
