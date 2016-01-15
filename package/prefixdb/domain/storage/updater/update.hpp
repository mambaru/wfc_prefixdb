#pragma once

#include <string>
#include <wfc/json.hpp>
namespace wamba{ namespace prefixdb{

// не нужно, определяем с parse
  /*
enum class json_type
{
  none,         // без типа, поля нет
  string,
  number,
  object,
  array,
  any           // любой тип
};
*/
struct json_type_json
{
  
};

enum class update_mode
{
  inc,
  packed
  /*,      // только для метода 'inc' simple inc, в raw число * неа!! , работает с 100 и {"value":100}
  lua,     // Обработка луа, передаеться raw или как есть ??
// объеты { dt, raw } 
  set,     // форсированая замена c ttl  
  get,     // с обновлением ttl
  add,     // добавить в массив
  update,  // замена с проверкой типа
  inc,     // инкремент поля
  packed  // пакетное обновление с возможностью инкремента
  */
};


struct update_mode_json
{
  JSON_NAME(inc)
  JSON_NAME(packed)
  typedef ::wfc::json::enumerator<
    update_mode,
    ::wfc::json::member_list<
        ::wfc::json::enum_value<n_inc, update_mode, update_mode::inc>,
        ::wfc::json::enum_value<n_packed, update_mode, update_mode::packed>
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

struct persistent
{
  // json_type type = json_type::any; опредеяеться типом
  time_t dt = 0;    // death time (время смерти)
  std::string val;  // произвольный json объект
};

struct persistent_update: persistent
{
  bool force = true;
};

struct persistent_json
{
  JSON_NAME(dt)
  JSON_NAME(val)
  
  typedef ::wfc::json::object<
    persistent,
    ::wfc::json::member_list<
        ::wfc::json::member<n_dt, persistent, time_t, &persistent::dt>,
        ::wfc::json::member<n_val, persistent, std::string, &persistent::val, ::wfc::json::raw_value<> >
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

struct persistent_update_json
{
  JSON_NAME(ttl)
  JSON_NAME(val)
  JSON_NAME(force)
  
  typedef ::wfc::json::object<
    persistent_update,
    ::wfc::json::member_list<
        ::wfc::json::member<n_force, persistent_update, bool, &persistent_update::force>,
        ::wfc::json::member<n_ttl, persistent, time_t, &persistent::dt>,
        ::wfc::json::member<n_val, persistent, std::string, &persistent::val, ::wfc::json::raw_value<> >
    >
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};


struct update
{
  update_mode mode;
  std::string value;
};

struct update_json
{
  JSON_NAME(m)
  JSON_NAME(v)

  // dt -> ttl
  // json_type определяется режимом и raw
  typedef ::wfc::json::object<
    update,
    ::wfc::json::member_list<
        ::wfc::json::member<n_m, update, update_mode, &update::mode, update_mode_json>,
        ::wfc::json::member<n_v, update, std::string, &update::value, ::wfc::json::raw_value<> >
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
