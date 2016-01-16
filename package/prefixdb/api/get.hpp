#pragma once

#include <prefixdb/api/set.hpp>
#include <prefixdb/api/aux/common_status.hpp>

namespace wamba { namespace prefixdb {

namespace request
{
  struct get
  {
    typedef key_list_t field_list_t;
    bool noval = false; // не сериализуеться 
    std::string prefix;
    field_list_t fields;
    typedef std::unique_ptr<get> ptr;
  };
}

namespace response
{
  struct get
  {
    /*
    struct field
      : basic_field
    {};
    typedef std::vector<field> field_list_t;
    */
    typedef raw_field_list_t field_list_t;
    
    common_status status = common_status::OK ;
    std::string prefix;
    field_list_t fields;

    typedef std::unique_ptr<get> ptr;
    typedef std::function< void(ptr) > handler;
  };

}

}}
