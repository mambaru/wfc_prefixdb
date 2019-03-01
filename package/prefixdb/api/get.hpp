#pragma once

#include <prefixdb/api/common_status.hpp>
#include <prefixdb/api/fields.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  struct get
  {
    typedef key_list_t field_list_t;
    bool noval = false; // не сериализуеться 
    std::string prefix;
    field_list_t fields;
    size_t snapshot = 0;
    typedef std::unique_ptr<get> ptr;
  };
}

namespace response
{
  struct get
  {
    typedef raw_field_list_t field_list_t;
    
    common_status status = common_status::OK ;
    std::string prefix;
    field_list_t fields;

    typedef std::unique_ptr<get> ptr;
    typedef std::function< void(ptr) > handler;
  };

}

}}
