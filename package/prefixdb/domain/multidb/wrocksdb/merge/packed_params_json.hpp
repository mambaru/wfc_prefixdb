#pragma once

#include "packed_params.hpp"
#include <wjson/wjson.hpp>
#include <string>

namespace wamba{ namespace prefixdb{

struct packed_field_params_json
{
  JSON_NAME(inc)
  JSON_NAME(val)

  typedef wjson::object<
    packed_field_params,
    wjson::member_list<
        wjson::member<n_inc, packed_field_params, std::string, &packed_field_params::inc, wjson::raw_value<> >,
        wjson::member<n_val, packed_field_params, std::string, &packed_field_params::val, wjson::raw_value<> >
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

typedef wjson::dict_vector< packed_field_params_json, 10 > packed_params_json;

}}
