#pragma once

#include <prefixdb/api/aux/common_status.hpp>
#include <prefixdb/api/aux/fields.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  struct packed
  {
    typedef raw_field_list_t field_list_t;
    std::string prefix;
    bool sync = false;
    bool nores = true;  // no result пустой результат, prefix="", status=OK
    bool noval = false; 
    field_list_t fields;

    typedef std::unique_ptr<packed> ptr;
  };
}

namespace response
{
  struct packed
  {
    typedef raw_field_list_t field_list_t;
    
    common_status status =  common_status::OK;
    std::string prefix;
    field_list_t fields;

    typedef std::unique_ptr<packed> ptr;
    typedef std::function< void(ptr) > handler;
  };
}

}}