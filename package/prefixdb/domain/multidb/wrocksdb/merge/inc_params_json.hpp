#pragma once

#include "inc_params.hpp"
#include <wjson/wjson.hpp>
#include <string>

namespace wamba{ namespace prefixdb{

struct inc_params_json
{
  JSON_NAME(inc)
  JSON_NAME(val)

  typedef wjson::object<
    inc_params,
    wjson::member_list<
        wjson::member<n_inc, inc_params, std::string, &inc_params::inc, wjson::raw_value<> >,
        wjson::member<n_val, inc_params, std::string, &inc_params::val, wjson::raw_value<> >
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};


}}
