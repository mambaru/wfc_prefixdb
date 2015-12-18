#pragma once

#include <prefixdb/api/aux/basic_field.hpp>
#include <prefixdb/api/aux/common_status.hpp>

namespace wamba { namespace prefixdb {

namespace request
{
  struct upd
  {
    struct params
    {
      std::string key;
      int inc = 0;
      int def = 0;
    };
    
    struct field: field_base
    {
      typedef std::vector<params> params_list_t;
      params_list_t val;
    };
    
    typedef std::vector<field> field_list_t;
    
    std::string prefix;
    bool nores = true;                  // no result пустой результат, prefix="", status=OK
    bool noval = true;
    field_list_t fields;
    typedef std::unique_ptr<upd> ptr;
  };
}

namespace response
{
  struct upd
  {
    struct field
      : basic_field
    {
    };
    typedef std::vector<field> field_list_t;
    
    common_status status =  common_status::OK;
    std::string prefix;
    field_list_t fields;

    typedef std::unique_ptr<upd> ptr;
    typedef std::function< void(ptr) > handler;
  };
}

}}
