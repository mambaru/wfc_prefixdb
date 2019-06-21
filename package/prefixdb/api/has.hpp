#pragma once

#include <prefixdb/api/common_status.hpp>
#include <prefixdb/api/fields.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  struct has
  {
    bool noval = true; // не сериализуеться
    std::string prefix;
    key_list_t fields;
    size_t snapshot = 0;
    typedef std::unique_ptr<has> ptr;
  };
}

namespace response
{
  struct has
  {
    
    common_status status = common_status::OK;
    std::string prefix;
    field_list_t fields;

    typedef std::unique_ptr<has> ptr;
    typedef std::function< void(ptr) > handler;
  };

}

}}
