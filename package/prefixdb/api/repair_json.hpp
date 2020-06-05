#pragma once

#include <prefixdb/api/common_status.hpp>
#include <prefixdb/api/fields.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  struct repair_json
  {
    /* Если префикс не указан, то восстановления для всех и по полной.
     * Ответ всегда пустой, результат в логах, остальные параметры игнорируются
     */
    std::string prefix;
    bool nores = true;
    bool noval = false;
    bool sync = false;
    std::string from;
    std::string to;
    bool beg = true;
    size_t offset = 0;
    size_t limit = 0; // 0 - полное восстановление
    size_t snapshot = 0;
    typedef std::unique_ptr<repair_json> ptr;
  };
}

namespace response
{
  struct repair_json
  {
    std::string prefix;
    common_status status = common_status::OK ;
    bool fin = true;
    size_t total = 0;
    size_t repaired = 0;
    field_list_t fields;
    // ключ для следующего запроса
    std::string last_key;

    typedef std::unique_ptr<repair_json> ptr;
    typedef std::function< void(ptr) > handler;
  };
}

}}
