#pragma once

#include <prefixdb/api/release_snapshot.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct release_snapshot_json
  {
    JSON_NAME(prefix)
    JSON_NAME(snapshot)
    
    
    typedef wjson::object<
      release_snapshot,
      wjson::member_list<
        wjson::member<n_prefix, release_snapshot, std::string, &release_snapshot::prefix >,
        wjson::member<n_snapshot, release_snapshot, size_t, &release_snapshot::snapshot >
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

namespace response
{
  struct release_snapshot_json
  {
    JSON_NAME(prefix)
    JSON_NAME(status)

    typedef wjson::object<
      release_snapshot,
      wjson::member_list<
        wjson::member<n_prefix, release_snapshot, std::string, &release_snapshot::prefix >,
        wjson::member<n_status, release_snapshot, common_status, &release_snapshot::status, common_status_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
