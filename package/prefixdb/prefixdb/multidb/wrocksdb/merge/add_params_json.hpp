#pragma once

#include "add_params.hpp"
#include <wfc/json.hpp>
#include <string>

namespace wamba{ namespace prefixdb{

struct add_params_json
{
  JSON_NAME(lim)
  JSON_NAME(arr)

  typedef ::wfc::json::object<
    add_params,
    ::wfc::json::member_list<
        ::wfc::json::member<n_lim, add_params, size_t, &add_params::lim >,
        ::wfc::json::member<n_arr, add_params, std::vector<std::string>, &add_params::arr, 
                                   ::wfc::json::array< std::vector< ::wfc::json::raw_value<> >, 20 > >
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
