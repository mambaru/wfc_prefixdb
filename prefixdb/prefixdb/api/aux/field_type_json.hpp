#pragma once

#include <prefixdb/api/aux/field_type.hpp>
#include <wfc/json.hpp>
namespace wamba { namespace prefixdb {

struct field_type_json
{
  JSON_NAME(none)
  JSON_NAME(string)
  JSON_NAME(number)
  JSON_NAME(object)
  JSON_NAME(array)
  JSON_NAME(any)
  
  typedef ::wfc::json::enumerator<
    field_type,
    ::wfc::json::member_list<
        ::wfc::json::enum_value<n_none, field_type, field_type::none>,
        ::wfc::json::enum_value<n_string, field_type, field_type::string>,
        ::wfc::json::enum_value<n_number, field_type, field_type::number>,
        ::wfc::json::enum_value<n_object, field_type, field_type::object>,
        ::wfc::json::enum_value<n_array, field_type, field_type::array>,
        ::wfc::json::enum_value<n_any, field_type, field_type::any>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
