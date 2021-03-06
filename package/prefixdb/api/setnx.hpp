#pragma once

#include <prefixdb/api/common_status.hpp>
#include <prefixdb/api/fields.hpp>
#include <memory>
#include <string>

namespace wamba { namespace prefixdb {

namespace request
{
  struct setnx
  {
    std::string prefix;
    field_list_t fields;

    bool sync = false;
    bool nores = true;
    bool noval = false;
    // Только для noval=false, пишет в базу, а значение из snapshot_id
    size_t snapshot = 0;
    typedef std::unique_ptr<setnx> ptr;
  };
}

namespace response
{
  struct setnx
  {
    common_status status =  common_status::OK;
    std::string prefix;
    field_list_t fields;

    typedef std::unique_ptr<setnx> ptr;
    typedef std::function< void(ptr) > handler;
  };

}

}}
