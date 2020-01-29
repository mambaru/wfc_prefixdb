#pragma once

#include <prefixdb/api/common_status.hpp>
#include <prefixdb/api/fields.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  /**
   * @brief Установка значений для нескольких ключей для заданного префикса.
   * @details Значение ключа - произвольный json-объект, строка, число или флаг. Объект сохраняется в базу как есть, 
   * без десериализации, в таком же виде и возвращается при запросе.
   * @see @ref request::set_json "Запрос 'set' в формате JSON"
   * @see @ref response::set "Ответ на запрос 'set'"
   */
  struct set
  {
    /**
     * @brief Префикс
     * @details Обязательный параметр. Если префикс не существует, то создает новый.
     * @return @ref common_status::EmptyPrefix "EmptyPrefix" - если не задан префикс или пустая строка
     * @return @ref common_status::CreatePrefixFail "CreatePrefixFail" - не смог создать новый префикс 
     * @return @ref common_status::PrefixLengthExceeded "PrefixLengthExceeded" - превышен допустимый размер в имени префикса
     */
    std::string prefix;  

    /**
     * @brief список пар ключ-значение для записи
     * @return Ошибка @ref common_status::TooManyKeys "TooManyKeys"
     * @return Ошибка @ref common_status::KeyLengthExceeded "KeyLengthExceeded"
     * @return Ошибка @ref common_status::ValueLengthExceeded "ValueLengthExceeded"
     */
    field_list_t fields; 

    /**
     * @brief Синхронная запись на диск (без кэширования)
     * @details Устанавливает соответствующий флаг при записи в RockDB, а также игнорирует отложенную запись через очередь, 
     * если такая разрешена в конфигурации @ref db_config::enable_delayed_write.
     */
    bool sync = false;

    /**
     * @brief TODO
     */
    bool nores = true;

    /**
     * @brief TODO
     */
    bool noval = false;

    /**
     * @brief TODO
     */
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
    /**
     * @brief стаус ответа
     */
    common_status status =  common_status::OK;
    
    /**
     * @brief префикс
     */
    std::string prefix;
    
    
    /**
     * @brief Список установленных полей (значение в зависимости от флагов @ref request::set::noval "noval" и @ref request::set::nores "nores")
     */
    field_list_t fields;

    typedef std::unique_ptr<set> ptr;

    typedef std::function< void(ptr) > handler;
  };

}

}}
