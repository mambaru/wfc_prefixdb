#pragma once

#include <string>
#include <wfc/json.hpp>

namespace wamba{ namespace prefixdb{

enum class merge_mode
{
  inc,
  packed
};


struct merge_mode_json
{
  JSON_NAME(inc)
  JSON_NAME(packed)
  typedef ::wfc::json::enumerator<
    merge_mode,
    ::wfc::json::member_list<
        ::wfc::json::enum_value<n_inc, merge_mode, merge_mode::inc>,
        ::wfc::json::enum_value<n_packed, merge_mode, merge_mode::packed>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

struct merge
{
  merge_mode mode;
  std::string value;
};

struct merge_json
{
  JSON_NAME(m)
  JSON_NAME(v)

  typedef ::wfc::json::object<
    merge,
    ::wfc::json::member_list<
        ::wfc::json::member<n_m, merge, merge_mode,  &merge::mode, merge_mode_json>,
        ::wfc::json::member<n_v, merge, std::string, &merge::value, ::wfc::json::raw_value<> >
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};


struct inc_params
{
  std::string key;
  std::string inc = "null";
  std::string val = "null";
};

struct inc_params_json
{
  JSON_NAME(key)
  JSON_NAME(inc)
  JSON_NAME(val)

  // dt -> ttl
  // json_type определяется режимом и raw
  typedef ::wfc::json::object<
    inc_params,
    ::wfc::json::member_list<
        ::wfc::json::member<n_key, inc_params, std::string, &inc_params::key >,
        ::wfc::json::member<n_inc, inc_params, std::string, &inc_params::inc, ::wfc::json::raw_value<> >,
        ::wfc::json::member<n_val, inc_params, std::string, &inc_params::val, ::wfc::json::raw_value<> >
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

//typedef std::pair<std::string, inc_params> inc_field;
typedef std::vector< inc_params > packed_params_t;

typedef ::wfc::json::array< std::vector<inc_params_json> > packed_params_json;
/*
typedef ::wfc::json::object2array<
  ::wfc::json::value<std::string>,
  inc_params_json,
  10
> packed_params_json;
*/

typedef std::pair<std::string, std::string> packed_field;
typedef std::vector< packed_field > packed_t;

typedef ::wfc::json::object2array<
  ::wfc::json::value<std::string>,
  ::wfc::json::raw_value<std::string>,
  10
> packed_json;

}}
