#pragma once

#include <prefixdb/api/aux/basic_field.hpp>
#include <prefixdb/api/aux/common_status.hpp>

namespace wamba { namespace prefixdb {

typedef std::pair<std::string, std::string> field_pair;
typedef std::vector< field_pair > raw_field_list_t;
typedef std::vector< std::string > key_list_t;

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
