#pragma once

#include <prefixdb/api/backup.hpp>
#include <prefixdb/api/aux/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct backup_json
  {
    JSON_NAME(prefix)
    JSON_NAME(sync)
    JSON_NAME(path)

    typedef ::wfc::json::object<
      backup,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefix, backup, std::vector<std::string>, &backup::prefixes, ::wfc::json::array_of_strings >,
        ::wfc::json::member<n_path,   backup, std::string, &backup::path>,
        ::wfc::json::member<n_sync,   backup, bool, &backup::sync>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

namespace response
{
  struct backup_json
  {
    JSON_NAME(status_map)
    JSON_NAME(status)
    JSON_NAME(path)

    typedef ::wfc::json::object<
      backup,
      ::wfc::json::member_list<
        ::wfc::json::member<n_path, backup, std::string, &backup::path>,
        ::wfc::json::member<n_status, backup, common_status, &backup::status, common_status_json>,
        ::wfc::json::member<n_status_map, backup, backup::status_map_t, &backup::status_map, raw_fields_list_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
