#pragma once

#include <prefixdb/api/common_status.hpp>
#include <prefixdb/api/fields.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  /**
   * @brief Установка значений для нескольких ключей.
   * @details Значение ключа - произвольный json-объект, строка, число или флаг. Объект сохраняется в базу как есть, 
   * без десериализации, в таком же виде и возвращается при запросе. JSON @ref request::set_json
   * @see request::set_json
   * @see response::set
   */
  struct set
  {
   
    bool sync = false;   ///< синхронная запись на диск (без кэширования)
    bool nores = true;
    bool noval = false;
    std::string prefix;  ///< имя префикса
    field_list_t fields; ///< список пар ключ-значение для записи
    // Только для noval=false, пишет в базу, а значение из snapshot_id
    size_t snapshot = 0;
    typedef std::unique_ptr<set> ptr;
  };
}

namespace response
{
  /**
   * @brief Ответ Установка значений для нескольких ключей.
   * @details -Значение ключа - произвольный json-объект, строка, число или флаг. Объект сохраняется в базу как есть, 
   * без десериализации, в таком же виде и возвращается при запросе-. JSON @ref response::set_json
   * @see response::set_json - JSON
   * @see request::set - запрос
   */
  struct set
  {
    common_status status =  common_status::OK;
    std::string prefix;
    field_list_t fields;

    typedef std::unique_ptr<set> ptr;
    typedef std::function< void(ptr) > handler;
  };

}

}}
