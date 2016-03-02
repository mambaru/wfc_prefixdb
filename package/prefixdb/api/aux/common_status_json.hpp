#pragma once

#include <prefixdb/api/aux/common_status.hpp>
#include <wfc/json.hpp>
namespace wamba { namespace prefixdb {

struct common_status_json
{
  JSON_NAME(OK)
  JSON_NAME(TransactLogError)
  JSON_NAME(EmptyFields)
  JSON_NAME(EmptyPrefix)
  JSON_NAME(WriteError)
  JSON_NAME(CreatePrefixFail)
  JSON_NAME(SomeFieldFail)
  JSON_NAME(PrefixNotFound)
  JSON_NAME(TooManyKeys)
  
  JSON_NAME(KeyLengthExceeded)
  JSON_NAME(ValueLengthExceeded)
  JSON_NAME(PrefixLengthExceeded)
  
  typedef ::wfc::json::enumerator<
    common_status,
    ::wfc::json::member_list<
        ::wfc::json::enum_value<n_OK, common_status, common_status::OK>,
        ::wfc::json::enum_value<n_TransactLogError, common_status, common_status::TransactLogError>,
        ::wfc::json::enum_value<n_EmptyFields, common_status, common_status::EmptyFields>,
        ::wfc::json::enum_value<n_EmptyPrefix, common_status, common_status::EmptyPrefix>,
        ::wfc::json::enum_value<n_CreatePrefixFail, common_status, common_status::CreatePrefixFail>,
        ::wfc::json::enum_value<n_WriteError, common_status, common_status::WriteError>,
        ::wfc::json::enum_value<n_SomeFieldFail, common_status, common_status::SomeFieldFail>,
        ::wfc::json::enum_value<n_PrefixNotFound, common_status, common_status::PrefixNotFound>,
        ::wfc::json::enum_value<n_TooManyKeys, common_status, common_status::TooManyKeys>,
        ::wfc::json::enum_value<n_KeyLengthExceeded, common_status, common_status::KeyLengthExceeded>,
        ::wfc::json::enum_value<n_ValueLengthExceeded, common_status, common_status::ValueLengthExceeded>,
        ::wfc::json::enum_value<n_PrefixLengthExceeded, common_status, common_status::PrefixLengthExceeded>
        /*, ::wfc::json::enum_value<n_TooManyPrefixes, common_status, common_status::TooManyPrefixes>*/
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
