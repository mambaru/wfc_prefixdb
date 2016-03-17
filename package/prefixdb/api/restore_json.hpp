#pragma once

#include <prefixdb/api/restore.hpp>
#include <prefixdb/api/aux/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

  
namespace request 
{
  struct restore_json
  {
    JSON_NAME(prefix)
    JSON_NAME(sync)
    JSON_NAME(path)

    typedef ::wfc::json::object<
      restore,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefix, restore, std::vector<std::string>, &restore::prefixes, ::wfc::json::array_of_strings >,
        ::wfc::json::member<n_path,   restore, std::string, &restore::path>,
        ::wfc::json::member<n_sync,   restore, bool, &restore::sync>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

namespace response
{
  struct restore_json
  {
    JSON_NAME(status_map)
    JSON_NAME(status)
    JSON_NAME(path)

    typedef ::wfc::json::object<
      restore,
      ::wfc::json::member_list<
        ::wfc::json::member<n_path, restore, std::string, &restore::path>,
        ::wfc::json::member<n_status, restore, common_status, &restore::status, common_status_json>,
        ::wfc::json::member<n_status_map, restore, restore::status_map_t, &restore::status_map, raw_fields_list_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
