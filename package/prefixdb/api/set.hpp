#pragma once

#include <prefixdb/api/common_status.hpp>
#include <prefixdb/api/fields.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  /**
   * @brief Параметры дляустановка значений для нескольких ключей для заданного префикса.
   * @details Значение ключа - произвольный json-объект, строка, число или флаг. Объект сохраняется в базу как есть, 
   * без десериализации, в таком же виде и возвращается при запросе.
   * @see @ref request::set_json "Запрос @ref request::set в формате JSON"
   * @see @ref response::set "Ответ на запрос @ref request::set"
   */
  struct set
  {
    /**
     * @brief Префикс
     * @details Обязательный параметр. Если БД префикса не существует, то она создается автоматически.
     * @retval response::set::status @ref common_status::EmptyPrefix "EmptyPrefix" - если не задан префикс или пустая строка
     * @retval response::set::status @ref common_status::CreatePrefixFail "CreatePrefixFail" - не смог создать новый префикс 
     * @retval response::set::status @ref common_status::PrefixLengthExceeded "PrefixLengthExceeded" - превышен допустимый размер в имени префикса
     */
    std::string prefix;  

    /**
     * @brief список пар ключ-значение для записи
     * @retval response::set::status @ref common_status::TooManyKeys "TooManyKeys"
     * @retval response::set::status @ref common_status::KeyLengthExceeded "KeyLengthExceeded"
     * @retval response::set::status @ref common_status::ValueLengthExceeded "ValueLengthExceeded"
     */
    field_list_t fields; 

    /**
     * @brief Синхронная запись на диск (без кэширования)
     * @details Устанавливает соответствующий флаг при записи в RockDB, а также игнорирует отложенную запись через очередь, 
     * если такая разрешена в конфигурации @ref db_config::enable_delayed_write.
     */
    bool sync = false;

    /**
     * @brief Отладочный флаг для чтения после записи.
     * @defalts
     * @details если **true** (по умолчанию), то пустой результат в ответе. В противном случае происходит вычитывание всех полей,
     * которые были записаны и возврат значения каждого поля (**noval**==**false**) или статус чтения **true**|**false** (**noval**==**true**)
     * Если задан **nores**==**true**, то **noval** игнорируется
     */
    bool nores = true;

    /**
     * @brief Отладочный флаг для управлением чтения после записи.
     * @details Действет только если **nores**==false. Если задан, но вы результате запроса будут записаны вместо значения ключей
     * статус чтения (true или false)
     */
    bool noval = false;

    /**
     * @brief Отладочный флаг для чтения после записи из указанного среза
     * @details Действет только если **nores**==false
     * @see @ref request::create_snapshot - для создания среза
     */
    size_t snapshot = 0;

    /**
     * @brief Тип указателя для удобного использования в интерфейсах
     */
    typedef std::unique_ptr<set> ptr;
    
    static set create_schema() 
    {
      set s;
      s.prefix = "prefix1";
      s.fields.emplace_back(std::make_pair("\"key1\"", "\"value1\""));
      s.fields.emplace_back(std::make_pair("\"key2\"", "[1,2,3]"));
      s.fields.emplace_back(std::make_pair("\"key3\"", "{}"));
      return s;
    }
  };
}

namespace response
{
  /**
   * @brief Ответ запрос @ref request::set
   * @details Возвращает статус записи и, если отключен @ref request::set::nores, список записанных полей
   * @see @ref response::set_json 
   * @see @ref request::set 
   */
  struct set
  {
    /**
     * @brief стаус ответа @ref common_status::OK если успех
     * @see @ref common_status
     */
    common_status status =  common_status::OK;
    
    /**
     * @brief префикс, который был задан в запросе
     */
    std::string prefix;
    
    /**
     * @brief Список установленных полей и из значений
     * @details Инициалзируется в зависимости от значений флагов @ref request::set::noval и @ref request::set::nores в запросе
     */
    field_list_t fields;

    /**
     * @brief Тип указателя для удобного использования в методах API
     */
    typedef std::unique_ptr<set> ptr;

    /**
     * @brief Тип функции обратного вызова обработки ответа в методах API
     */
    typedef std::function< void(ptr) > handler;
  };

}

}}
