#pragma once

#include <wfc/iinterface.hpp>
#include <prefixdb/api/set.hpp>
#include <prefixdb/api/setnx.hpp>
#include <prefixdb/api/get.hpp>
#include <prefixdb/api/has.hpp>
#include <prefixdb/api/del.hpp>
#include <prefixdb/api/inc.hpp>
#include <prefixdb/api/add.hpp>
#include <prefixdb/api/packed.hpp>
#include <prefixdb/api/range.hpp>
#include <prefixdb/api/get_updates_since.hpp>
#include <prefixdb/api/get_all_prefixes.hpp>
#include <prefixdb/api/detach_prefixes.hpp>
#include <prefixdb/api/attach_prefixes.hpp>
#include <prefixdb/api/delay_background.hpp>
#include <prefixdb/api/continue_background.hpp>
#include <prefixdb/api/compact_prefix.hpp>
#include <prefixdb/api/create_snapshot.hpp>
#include <prefixdb/api/release_snapshot.hpp>

namespace wamba { namespace prefixdb{

/**
  * @brief Интерфейс к PrefixDB
  * @details 
  * 
  * Основные операции
  *   Метод    |  Запрос (JSON)  |  Ответ (JSON)  | Кратко
  * -----------|-----------------|----------------|------------
  * @ref iprefixdb::set "set" | @ref request::set (@ref request::set_json "json") | @ref response::set (@ref response::set_json "json") | Запись
  * @ref iprefixdb::get "get" | @ref request::get (@ref request::get_json "json") | @ref response::get (@ref response::get_json "json") | Чтение
  * @ref iprefixdb::del "del" | @ref request::del (@ref request::del_json "json") | @ref response::del (@ref response::del_json "json") | Удаление
  * @ref iprefixdb::has "has" | @ref request::has (@ref request::has_json "json") | @ref response::has (@ref response::has_json "json") | Проверка
  * 
  * Операции слияния
  *   Метод    |  Запрос (JSON)  |  Ответ (JSON)  | Кратко
  * -----------|-----------------|----------------|------------
  * @ref iprefixdb::inc "inc" | @ref request::inc (@ref request::inc_json "json") | @ref response::inc (@ref response::inc_json "json") | Целочисленный счетчик 
  * @ref iprefixdb::add "add" | @ref request::add (@ref request::add_json "json") | @ref response::add (@ref response::add_json "json") | Циклический массив
  * @ref iprefixdb::setnx "setnx" | @ref request::setnx (@ref request::setnx_json "json") | @ref response::setnx (@ref response::setnx_json "json") | Записать, если не существует 
  * @ref iprefixdb::packed "packed" | @ref request::packed (@ref request::packed_json "json") | @ref response::packed (@ref response::packed_json "json") | Пакетное обновление полей JSON-объекта
  * 
  * Работа с диапазоном
  *   Метод    |  Запрос (JSON)  |  Ответ (JSON)  | Кратко
  * -----------|-----------------|----------------|------------
  *     range    | @ref request::range (@ref request::range_json) | @ref response::range (@ref response::range_json) | Запись
  *
  * Префиксы
  *   Метод    |  Запрос (JSON)  |  Ответ (JSON)  | Кратко
  * -----------|-----------------|----------------|------------
  * get_all_prefixes | @ref request::get_all_prefixes (@ref request::get_all_prefixes_json) | @ref response::get_all_prefixes (@ref response::get_all_prefixes_json) | Запись
  * detach_prefixes | @ref request::set (@ref request::set_json) | @ref response::set (@ref response::set_json) | Запись
  * attach_prefixes | @ref request::set (@ref request::set_json) | @ref response::set (@ref response::set_json) | Запись
  * compact_prefix | @ref request::set (@ref request::set_json) | @ref response::set (@ref response::set_json) | Запись
  *
  * Снапшоты
  *   Метод    |  Запрос (JSON)  |  Ответ (JSON)  | Кратко
  * -----------|-----------------|----------------|------------
  * create_snapshot | @ref request::set (@ref request::set_json) | @ref response::set (@ref response::set_json) | Запись
  * release_snapshot | @ref request::set (@ref request::set_json) | @ref response::set (@ref response::set_json) | Запись
  * 
  * Репликация и бэкапы
  *   Метод    |  Запрос (JSON)  |  Ответ (JSON)  | Кратко
  * -----------|-----------------|----------------|------------
  * delay_background | @ref request::set (@ref request::set_json) | @ref response::set (@ref response::set_json) | Запись
  * continue_background | @ref request::set (@ref request::set_json) | @ref response::set (@ref response::set_json) | Запись
  * get_updates_since | @ref request::set (@ref request::set_json) | @ref response::set (@ref response::set_json) | Запись
  */
struct iprefixdb: public ::wfc::iinterface
{
  virtual ~iprefixdb() {}
  

  /**
    * @brief Установка значений для нескольких ключей.
    * @param req запрос
    * @param cb обработчик обратного вызова
    */
  virtual void set( request::set::ptr req, response::set::handler cb) = 0;
  virtual void get( request::get::ptr req, response::get::handler cb) = 0;
  virtual void del( request::del::ptr req, response::del::handler cb) = 0;
  virtual void has( request::has::ptr req, response::has::handler cb) = 0;

  virtual void inc( request::inc::ptr req, response::inc::handler cb) = 0;
  virtual void add( request::add::ptr req, response::add::handler cb) = 0;
  virtual void setnx( request::setnx::ptr req, response::setnx::handler cb) = 0;
  virtual void packed( request::packed::ptr req, response::packed::handler cb) = 0;

  virtual void range( request::range::ptr req, response::range::handler cb) = 0;

  virtual void get_all_prefixes( request::get_all_prefixes::ptr req, response::get_all_prefixes::handler cb) = 0;
  virtual void detach_prefixes( request::detach_prefixes::ptr req, response::detach_prefixes::handler cb) = 0;
  virtual void attach_prefixes( request::attach_prefixes::ptr req, response::attach_prefixes::handler cb) = 0;
  virtual void compact_prefix( request::compact_prefix::ptr req, response::compact_prefix::handler cb) = 0;

  virtual void create_snapshot( request::create_snapshot::ptr req, response::create_snapshot::handler cb) = 0;
  virtual void release_snapshot( request::release_snapshot::ptr req, response::release_snapshot::handler cb) = 0;


  virtual void delay_background( request::delay_background::ptr req, response::delay_background::handler cb) = 0;
  virtual void continue_background( request::continue_background::ptr req, response::continue_background::handler cb) = 0;
  virtual void get_updates_since( request::get_updates_since::ptr req, response::get_updates_since::handler cb) = 0;
  
  
};

}}
