#pragma once

#include <prefixdb/api/get_updates_since.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct get_updates_since_json
  {
    JSON_NAME(prefix)
    JSON_NAME(seq)
    JSON_NAME(limit)
    
    typedef wjson::object<
      get_updates_since,
      wjson::member_list<
        wjson::member<n_prefix, get_updates_since, std::string, &get_updates_since::prefix>,
        wjson::member<n_seq, get_updates_since, size_t, &get_updates_since::seq>,
        wjson::member<n_limit, get_updates_since, size_t, &get_updates_since::limit>
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

namespace response
{
  struct get_updates_since_json
  {
    JSON_NAME(prefix)
    JSON_NAME(seq_first)
    JSON_NAME(seq_last)
    JSON_NAME(seq_final)
    JSON_NAME(logs)
    JSON_NAME(status)

    typedef wjson::object<
      get_updates_since,
      wjson::member_list<
        wjson::member<n_prefix, get_updates_since, std::string, &get_updates_since::prefix>,
        wjson::member<n_seq_first, get_updates_since, size_t, &get_updates_since::seq_first>,
        wjson::member<n_seq_last, get_updates_since, size_t, &get_updates_since::seq_last>,
        wjson::member<n_seq_final, get_updates_since, size_t, &get_updates_since::seq_final>,
        wjson::member<n_logs, get_updates_since, get_updates_since::logs_type, &get_updates_since::logs, wjson::vector_of_strings<16> >,
        wjson::member<n_status, get_updates_since, common_status, &get_updates_since::status, common_status_json>
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

}}
