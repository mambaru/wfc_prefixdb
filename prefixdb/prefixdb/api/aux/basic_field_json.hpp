#pragma once

#include <prefixdb/api/aux/basic_field.hpp>
#include <prefixdb/api/aux/field_base_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

struct basic_field_json
{
  JSON_NAME(val)
  
  typedef ::wfc::json::object<
    basic_field,
    ::wfc::json::member_list<
      ::wfc::json::base<field_base_json>,
      ::wfc::json::member<n_val, basic_field, std::string, &basic_field::val>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
