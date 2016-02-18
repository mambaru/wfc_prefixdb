#pragma once

#include <prefixdb/api/aux/common_status.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  struct backup
  {
    // синхронный backup (ответ после записи backup)
    bool sync = false;
    bool nores = true;
    
    // если не задан, то все префиксы
    std::vector<std::string> prefixes;
    
    // Путь для backup отдельного префикса (если задан), или к папке
    // куда класть backup для каждого префикса (path/prefix)
    // Если не задан, то берется из конфига
    std::string path;
    typedef std::unique_ptr<backup> ptr;
  };
}

namespace response
{
  struct backup
  {
    typedef raw_field_list_t status_map_t;
    common_status status =  common_status::OK;
    std::string path;
    status_map_t status_map;
    typedef std::unique_ptr<backup> ptr;
    typedef std::function< void(ptr) > handler;
  };

}

}}
