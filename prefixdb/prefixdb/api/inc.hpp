#pragma once

#include <prefixdb/api/aux/basic_field.hpp>
#include <prefixdb/api/aux/common_status.hpp>

namespace wamba { namespace prefixdb {

namespace request
{
  struct inc
  {
    struct field: field_base
    {
      bool force = true;
      int inc = 0;
      int def = 0;
    };
    typedef std::vector<field> field_list_t;
    
    std::string prefix;
    bool nores = false;  // no result пустой результат, prefix="", status=OK
    field_list_t fields;
    
    typedef std::unique_ptr<inc> ptr;
  };
}

namespace response
{
  struct inc
  {
    struct field
      : basic_field
    {
    };
    typedef std::vector<field> field_list_t;
    
    common_status status =  common_status::OK;
    std::string prefix;
    field_list_t fields;

    typedef std::unique_ptr<inc> ptr;
    typedef std::function< void(ptr) > handler;
  };
}

}}
