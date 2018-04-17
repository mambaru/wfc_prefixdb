#pragma once

#include <prefixdb/api/create_snapshot.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct create_snapshot_json
  {
    JSON_NAME(prefix)
    JSON_NAME(release_timeout_s)
    
    
    typedef wfc::json::object<
      create_snapshot,
      wfc::json::member_list<
        wfc::json::member<n_prefix, create_snapshot, std::string, &create_snapshot::prefix >,
        wfc::json::member<n_release_timeout_s, create_snapshot, time_t, &create_snapshot::release_timeout_s >
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

namespace response
{
  struct create_snapshot_json
  {
    JSON_NAME(prefix)
    JSON_NAME(status)
    JSON_NAME(snapshot)
    JSON_NAME(last_seq)

    typedef wfc::json::object<
      create_snapshot,
      wfc::json::member_list<
        wfc::json::member<n_prefix, create_snapshot, std::string, &create_snapshot::prefix >,
        wfc::json::member<n_snapshot, create_snapshot, size_t, &create_snapshot::snapshot >,
        wfc::json::member<n_last_seq, create_snapshot, size_t, &create_snapshot::last_seq >,
        wfc::json::member<n_status, create_snapshot, common_status, &create_snapshot::status, common_status_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
