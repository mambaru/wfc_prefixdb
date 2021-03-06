#pragma once

#include <prefixdb/api/common_status.hpp>
#include <prefixdb/api/fields.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  struct add
  {
    std::string prefix;
    bool sync = false;
    bool nores = true;  // no result пустой результат, prefix="", status=OK
    bool noval = false; 
    field_list_t fields;
    // Только для noval=false, пишет в базу, а значение из snapshot_id
    size_t snapshot = 0;
    typedef std::unique_ptr<add> ptr;
  };
}

namespace response
{
  struct add
  {
 
    common_status status =  common_status::OK;
    std::string prefix;
    field_list_t fields;

    typedef std::unique_ptr<add> ptr;
    typedef std::function< void(ptr) > handler;
  };
}

}}
