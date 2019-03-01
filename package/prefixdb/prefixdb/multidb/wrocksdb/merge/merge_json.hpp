#pragma once

#include "merge.hpp"
#include <string>
#include <wfc/json.hpp>

namespace wamba{ namespace prefixdb{

struct merge_mode_json
{
  JSON_NAME(inc)
  JSON_NAME(add)
  JSON_NAME(packed)
  JSON_NAME(setnx)
  typedef ::wfc::json::enumerator<
    merge_mode,
    ::wfc::json::member_list<
        ::wfc::json::enum_value<n_inc, merge_mode, merge_mode::inc>,
        ::wfc::json::enum_value<n_add, merge_mode, merge_mode::add>,
        ::wfc::json::enum_value<n_packed, merge_mode, merge_mode::packed>,
        ::wfc::json::enum_value<n_setnx, merge_mode, merge_mode::setnx>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

struct merge_json
{
  JSON_NAME(m)
  JSON_NAME(v)

  typedef ::wfc::json::object<
    merge,
    ::wfc::json::member_list<
        ::wfc::json::member<n_m, merge, merge_mode,  &merge::mode, merge_mode_json>,
        ::wfc::json::member<n_v, merge, std::string, &merge::raw, ::wfc::json::raw_value<> >
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
