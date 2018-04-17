#pragma once

#include <prefixdb/api/common_status.hpp>
#include <prefixdb/api/fields.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  struct range 
  {
    bool noval = false;
    std::string prefix;
    std::string from;
    std::string to;
    size_t offset = 0;
    size_t limit = 10;
    size_t snapshot = 0;
    typedef std::unique_ptr<range> ptr;
  };
}

namespace response
{
  struct range
  {
    typedef raw_field_list_t field_list_t;

    bool fin = true;
    common_status status = common_status::OK ;
    std::string prefix;
    field_list_t fields;

    typedef std::unique_ptr<range> ptr;
    typedef std::function< void(ptr) > handler;
  };
}

}}
