#pragma once

#include <prefixdb/api/aux/common_status.hpp>
#include <wfc/json.hpp>
namespace wamba { namespace prefixdb {

struct common_status_json
{
  JSON_NAME(OK)
  JSON_NAME(InvalidFieldValue)
  JSON_NAME(EmptyFields)
  JSON_NAME(EmptyPrefix)
  JSON_NAME(CreatePrefixFail)
  JSON_NAME(CreateSnapshotFail)
  JSON_NAME(PrefixNotFound)
  JSON_NAME(SnapshotNotFound)
  JSON_NAME(TooManyKeys)
  JSON_NAME(KeyLengthExceeded)
  JSON_NAME(ValueLengthExceeded)
  JSON_NAME(PrefixLengthExceeded)
  JSON_NAME(RangeLimitExceeded)
  JSON_NAME(InvalidSeqNumber)
  JSON_NAME(CompactFail)
  
  typedef ::wfc::json::enumerator<
    common_status,
    ::wfc::json::member_list<
        ::wfc::json::enum_value<n_OK, common_status, common_status::OK>,
        ::wfc::json::enum_value<n_InvalidFieldValue, common_status, common_status::InvalidFieldValue>,
        ::wfc::json::enum_value<n_EmptyFields, common_status, common_status::EmptyFields>,
        ::wfc::json::enum_value<n_EmptyPrefix, common_status, common_status::EmptyPrefix>,
        ::wfc::json::enum_value<n_CreatePrefixFail, common_status, common_status::CreatePrefixFail>,
        ::wfc::json::enum_value<n_PrefixNotFound, common_status, common_status::PrefixNotFound>,
        ::wfc::json::enum_value<n_CreateSnapshotFail, common_status, common_status::CreateSnapshotFail>,
        ::wfc::json::enum_value<n_SnapshotNotFound, common_status, common_status::SnapshotNotFound>,
        ::wfc::json::enum_value<n_TooManyKeys, common_status, common_status::TooManyKeys>,
        ::wfc::json::enum_value<n_KeyLengthExceeded, common_status, common_status::KeyLengthExceeded>,
        ::wfc::json::enum_value<n_ValueLengthExceeded, common_status, common_status::ValueLengthExceeded>,
        ::wfc::json::enum_value<n_PrefixLengthExceeded, common_status, common_status::PrefixLengthExceeded>,
        ::wfc::json::enum_value<n_RangeLimitExceeded, common_status, common_status::RangeLimitExceeded>,
        ::wfc::json::enum_value<n_InvalidSeqNumber, common_status, common_status::InvalidSeqNumber>,
        ::wfc::json::enum_value<n_CompactFail, common_status, common_status::CompactFail>
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}

namespace std{
  inline std::ostream& operator<< (std::ostream& os, wamba::prefixdb::common_status cs)
  {
    wamba::prefixdb::common_status_json::serializer()(cs, ostreambuf_iterator<char>(os));
    return os;
  }
}
