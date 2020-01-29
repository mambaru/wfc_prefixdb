#pragma once

#include <prefixdb/api/common_status.hpp>
#include <prefixdb/api/fields.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  struct del
  {
    bool sync = false;
    bool nores = true; // < [true] не нужен результат, noval игнорируется
    bool noval = true; // < [true] если false - вернуть значения удаленных полей
    std::string prefix;
    key_list_t fields;
    // Только для noval=false, пишет в базу, а значение из snapshot_id
    size_t snapshot = 0;
    typedef std::unique_ptr<del> ptr;
  };
}

namespace response
{
  struct del
  {
    common_status status = common_status::OK ;
    std::string prefix;
    field_list_t fields;

    typedef std::unique_ptr<del> ptr;
    typedef std::function< void(ptr) > handler;
  };

}

}}
