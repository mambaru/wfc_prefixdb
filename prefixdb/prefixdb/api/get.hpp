#pragma once

#include <prefixdb/api/aux/basic_field.hpp>
#include <prefixdb/api/aux/common_status.hpp>

namespace wamba { namespace prefixdb {

namespace request
{
  struct get
  {
    typedef std::vector<key_field> field_list_t;
    std::string prefix;
    field_list_t fields;
    typedef std::unique_ptr<get> ptr;
  };
}

namespace response
{
  struct get
  {
    struct field
      : basic_field
    {};
    typedef std::vector<field> field_list_t;
    
    common_status status = common_status::OK ;
    std::string prefix;
    field_list_t fields;

    typedef std::unique_ptr<get> ptr;
    typedef std::function< void(ptr) > handler;
  };

}

}}
