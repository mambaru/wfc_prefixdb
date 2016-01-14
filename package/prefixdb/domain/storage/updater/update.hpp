#pragma once

#include <string>

namespace wamba{ namespace prefixdb{

// не нужно, определяем с parse
enum class json_type
{
  none,         // без типа, поля нет
  string,
  number,
  object,
  array,
  any           // любой тип
};

struct json_type_json
{
  
};

enum class update_mode
{
  si,      // только для метода 'inc' simple inc, в raw число /* неа!! , работает с 100 и {"value":100}*/
  lua,     // Обработка луа, передаеться raw или как есть ??
// объеты { dt, raw } 
  set,     // форсированая замена c ttl
  get,     // с обновлением ttl
  add,     // добавить в массив
  update,  // замена с проверкой типа
  inc,     // инкремент поля
  packed  // пакетное обновление с возможностью инкремента
};

struct update_mode_json
{
  
};

struct persistent
{
  // json_type type = json_type::any; опредеяеться типом
  time_t dt = 0;    // death time (время смерти)
  std::string value;  // произвольный json объект
};

struct persistent_json
{
};

struct persistent_update_json
{
  // dt -> ttl
  // 
};

struct update: persistent
{
  json_type type = json_type::any;
  update_mode mode = update_mode::set;
  bool force=true; // игнорировать json_type
};

struct update_json
{
  // dt -> ttl
  // json_type определяется режимом и raw
};

}}
