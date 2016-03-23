#pragma once

#include <prefixdb/api/get_updates_since.hpp>
#include <prefixdb/api/aux/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct get_updates_since_json
  {
    JSON_NAME(prefix)
    JSON_NAME(seq)
    JSON_NAME(limit)
    
    typedef ::wfc::json::object<
      get_updates_since,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefix, get_updates_since, std::string, &get_updates_since::prefix>,
        ::wfc::json::member<n_seq, get_updates_since, size_t, &get_updates_since::seq>,
        ::wfc::json::member<n_limit, get_updates_since, size_t, &get_updates_since::limit>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
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

    typedef ::wfc::json::object<
      get_updates_since,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefix, get_updates_since, std::string, &get_updates_since::prefix>,
        ::wfc::json::member<n_seq_first, get_updates_since, size_t, &get_updates_since::seq_first>,
        ::wfc::json::member<n_seq_last, get_updates_since, size_t, &get_updates_since::seq_last>,
        ::wfc::json::member<n_seq_final, get_updates_since, size_t, &get_updates_since::seq_final>,
        ::wfc::json::member<n_logs, get_updates_since, get_updates_since::logs_type, &get_updates_since::logs, ::wfc::json::array_of_strings<16> >,
        ::wfc::json::member<n_status, get_updates_since, common_status, &get_updates_since::status, common_status_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
