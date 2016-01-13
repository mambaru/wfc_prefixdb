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
      bool force = true; // true - response::list пустой 
    };
    typedef std::vector<field> field_list_t;
    
    std::string prefix;
    bool sync = false;
    bool nores = true;
    bool noval = false;
    field_list_t fields;
    
    typedef std::unique_ptr<set> ptr;
  };
}

namespace response
{
  struct set
  {
    struct field
      : basic_field 
    {
    };
    typedef std::vector<field> field_list_t;
    
    common_status status =  common_status::OK;
    std::string prefix;
    field_list_t fields;

    typedef std::unique_ptr<set> ptr;
    typedef std::function< void(ptr) > handler;
  };

}

}}
