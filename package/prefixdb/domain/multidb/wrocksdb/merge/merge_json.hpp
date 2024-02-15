#pragma once

#include "merge.hpp"
#include <string>
#include <wjson/wjson.hpp>

namespace wamba{ namespace prefixdb{

struct merge_mode_json
{
  JSON_NAME(inc)
  JSON_NAME(add)
  JSON_NAME(packed)
  JSON_NAME(setnx)
  typedef wjson::enumerator<
    merge_mode,
    wjson::member_list<
        wjson::enum_value<n_inc, merge_mode, merge_mode::inc>,
        wjson::enum_value<n_add, merge_mode, merge_mode::add>,
        wjson::enum_value<n_packed, merge_mode, merge_mode::packed>,
        wjson::enum_value<n_setnx, merge_mode, merge_mode::setnx>
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

  typedef wjson::object<
    merge,
    wjson::member_list<
        wjson::member<n_m, merge, merge_mode,  &merge::mode, merge_mode_json>,
        wjson::member<n_v, merge, std::string, &merge::raw, wjson::raw_value<> >
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
