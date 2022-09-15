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
    
    
    typedef wjson::object<
      create_snapshot,
      wjson::member_list<
        wjson::member<n_prefix, create_snapshot, std::string, &create_snapshot::prefix >,
        wjson::member<n_release_timeout_s, create_snapshot, time_t, &create_snapshot::release_timeout_s >
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
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

    typedef wjson::object<
      create_snapshot,
      wjson::member_list<
        wjson::member<n_prefix, create_snapshot, std::string, &create_snapshot::prefix >,
        wjson::member<n_snapshot, create_snapshot, size_t, &create_snapshot::snapshot >,
        wjson::member<n_last_seq, create_snapshot, size_t, &create_snapshot::last_seq >,
        wjson::member<n_status, create_snapshot, common_status, &create_snapshot::status, common_status_json>
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

}}
