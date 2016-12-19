#pragma once

#include "inc_params.hpp"
#include <wfc/json.hpp>
#include <string>

namespace wamba{ namespace prefixdb{

struct inc_params_json
{
  JSON_NAME(inc)
  JSON_NAME(val)

  typedef ::wfc::json::object<
    inc_params,
    ::wfc::json::member_list<
        ::wfc::json::member<n_inc, inc_params, std::string, &inc_params::inc, ::wfc::json::raw_value<> >,
        ::wfc::json::member<n_val, inc_params, std::string, &inc_params::val, ::wfc::json::raw_value<> >
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};


}}
