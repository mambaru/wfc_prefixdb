#pragma once

#include <prefixdb/api/aux/common_status.hpp>
#include <wfc/json.hpp>
namespace wamba { namespace prefixdb {

struct common_status_json
{
  JSON_NAME(OK)
  JSON_NAME(WriteError)
  JSON_NAME(SomeFieldFail)
  JSON_NAME(PrefixNotFound)
  
  typedef ::wfc::json::enumerator<
    common_status,
    ::wfc::json::member_list<
        ::wfc::json::enum_value<n_OK, common_status, common_status::OK>,
        ::wfc::json::enum_value<n_WriteError, common_status, common_status::WriteError>,
        ::wfc::json::enum_value<n_SomeFieldFail, common_status, common_status::SomeFieldFail>,
        ::wfc::json::enum_value<n_PrefixNotFound, common_status, common_status::PrefixNotFound>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
