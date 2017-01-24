#pragma once

#include <prefixdb/api/aux/common_status.hpp>
#include <prefixdb/api/aux/fields.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  struct set
  {
    typedef raw_field_list_t field_list_t;
    
    bool sync = false;
    bool nores = true;
    bool noval = false;
    std::string prefix;
    field_list_t fields;
    typedef std::unique_ptr<set> ptr;
  };
}

namespace response
{
  struct set
  {
    typedef raw_field_list_t field_list_t;
    common_status status =  common_status::OK;
    std::string prefix;
    field_list_t fields;

    typedef std::unique_ptr<set> ptr;
    typedef std::function< void(ptr) > handler;
  };

}

}}
