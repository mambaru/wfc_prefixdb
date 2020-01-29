#pragma once

#include "packed_params.hpp"
#include <wfc/json.hpp>
#include <string>

namespace wamba{ namespace prefixdb{

struct packed_field_params_json
{
  JSON_NAME(inc)
  JSON_NAME(val)

  typedef ::wfc::json::object<
    packed_field_params,
    ::wfc::json::member_list<
        ::wfc::json::member<n_inc, packed_field_params, std::string, &packed_field_params::inc, ::wfc::json::raw_value<> >,
        ::wfc::json::member<n_val, packed_field_params, std::string, &packed_field_params::val, ::wfc::json::raw_value<> >
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

typedef ::wfc::json::dict_vector<
  packed_field_params_json,
  10
> packed_params_json;


}}
