#pragma once

#include <prefixdb/api/aux/basic_field.hpp>
#include <prefixdb/api/aux/common_status.hpp>

namespace wamba { namespace prefixdb {

namespace request
{
  struct set
  {
    struct field: basic_field
    {
      bool strong = false; // false - response::list пустой 
    };

    bool easy = true; // пустой результат, prefix="", status=OK
    std::string prefix;
    std::vector<field> fields;
    typedef std::unique_ptr<set> ptr;
  };
}

namespace response
{
  struct set
  {
    struct field: field_base {};

    common_status status;
    std::string prefix;
    std::vector<field> list;

    typedef std::unique_ptr<set> ptr;
    typedef std::function< void(ptr) > handler;
  };

}

}}
