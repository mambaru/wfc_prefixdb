#pragma once

#include "add_params.hpp"
#include <wjson/wjson.hpp>
#include <string>

namespace wamba{ namespace prefixdb{

struct add_params_json
{
  JSON_NAME(lim)
  JSON_NAME(arr)

  typedef wjson::object<
    add_params,
    wjson::member_list<
      wjson::member<n_lim, add_params, size_t, &add_params::lim >,
      wjson::member<n_arr, add_params, std::vector<std::string>, &add_params::arr, wjson::vector_of< wjson::raw_value<>, 20 > >
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
