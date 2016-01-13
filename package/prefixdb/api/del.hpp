#pragma once

#include <prefixdb/api/aux/basic_field.hpp>
#include <prefixdb/api/aux/common_status.hpp>

namespace wamba { namespace prefixdb {

namespace request
{
  struct del
  {
    typedef std::vector<key_field> field_list_t;
    bool sync = false;
    bool nores = true; // < [true] не нужен результат, noval игнорируется
    bool noval = true; // < [true] если false - вернуть значения удаленных полей
    std::string prefix;
    field_list_t fields;
    typedef std::unique_ptr<del> ptr;
  };
}

namespace response
{
  struct del
  {
    struct field
      : basic_field
    {};
    typedef std::vector<field> field_list_t;
    common_status status = common_status::OK ;
    std::string prefix;
    field_list_t fields;

    typedef std::unique_ptr<del> ptr;
    typedef std::function< void(ptr) > handler;
  };

}

}}
