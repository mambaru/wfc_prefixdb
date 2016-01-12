  #pragma once

#include <prefixdb/api/aux/basic_field.hpp>
#include <prefixdb/api/aux/common_status.hpp>

namespace wamba { namespace prefixdb {

namespace request
{
  struct has
  {
    typedef std::vector<key_field> field_list_t;
    bool noval = true; // не сериализуеться
    std::string prefix;
    field_list_t fields;
    typedef std::unique_ptr<has> ptr;
  };
}

namespace response
{
  struct has
  {
    struct field
      : field_base
    {};
    typedef std::vector<field> field_list_t;
    
    common_status status = common_status::OK;
    std::string prefix;
    field_list_t fields;

    typedef std::unique_ptr<has> ptr;
    typedef std::function< void(ptr) > handler;
  };

}

}}
