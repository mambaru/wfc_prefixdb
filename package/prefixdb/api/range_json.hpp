#pragma once

#include <prefixdb/api/range.hpp>
#include <prefixdb/api/aux/fields_json.hpp>
#include <prefixdb/api/aux/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct range_json
  {
    JSON_NAME(prefix)
    JSON_NAME(noval)
    JSON_NAME(from)
    JSON_NAME(to)
    JSON_NAME(limit)
    JSON_NAME(offset)
    
    typedef ::wfc::json::object<
      range,
      ::wfc::json::member_list<
        ::wfc::json::member<n_prefix, range, std::string, &range::prefix>,
        ::wfc::json::member<n_noval, range, bool, &range::noval>,
        ::wfc::json::member<n_from, range, std::string, &range::from>,
        ::wfc::json::member<n_to, range, std::string, &range::to>,
        ::wfc::json::member<n_limit, range, size_t, &range::limit>,
        ::wfc::json::member<n_offset, range, size_t, &range::offset>
      >
    > type;
    
    typedef type::target target;
    typedef type::serializer serializer;
  };
}

namespace response
{
  struct range_json
  {
    JSON_NAME(prefix)
    JSON_NAME2(n_final, "final")
    JSON_NAME(status)
    JSON_NAME(fields)
    
    typedef ::wfc::json::object<
      range,
      ::wfc::json::member_list<
        ::wfc::json::member<n_final,  range, bool, &range::fin>,
        ::wfc::json::member<n_prefix, range, std::string, &range::prefix>,
        ::wfc::json::member<n_status, range, common_status, &range::status, common_status_json>,
        ::wfc::json::member<n_fields, range, range::field_list_t, &range::fields, raw_fields_list_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
