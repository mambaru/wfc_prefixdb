#pragma once

#include <prefixdb/api/common_status.hpp>
#include <wfc/json.hpp>
namespace wamba { namespace prefixdb {

struct common_status_json
{
  JSON_NAME(OK)
  JSON_NAME(IOError)
  JSON_NAME(InvalidFieldValue)
  JSON_NAME(EmptyFields)
  JSON_NAME(EmptyPrefix)
  JSON_NAME(CreatePrefixFail)
  JSON_NAME(PrefixReadonly)
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

  typedef wjson::enumerator<
    common_status,
    wjson::member_list<
        wjson::enum_value<n_OK, common_status, common_status::OK>,
        wjson::enum_value<n_IOError, common_status, common_status::IOError>,
        wjson::enum_value<n_InvalidFieldValue, common_status, common_status::InvalidFieldValue>,
        wjson::enum_value<n_EmptyFields, common_status, common_status::EmptyFields>,
        wjson::enum_value<n_EmptyPrefix, common_status, common_status::EmptyPrefix>,
        wjson::enum_value<n_PrefixReadonly, common_status, common_status::PrefixReadonly>,
        wjson::enum_value<n_CreatePrefixFail, common_status, common_status::CreatePrefixFail>,
        wjson::enum_value<n_PrefixNotFound, common_status, common_status::PrefixNotFound>,
        wjson::enum_value<n_CreateSnapshotFail, common_status, common_status::CreateSnapshotFail>,
        wjson::enum_value<n_SnapshotNotFound, common_status, common_status::SnapshotNotFound>,
        wjson::enum_value<n_TooManyKeys, common_status, common_status::TooManyKeys>,
        wjson::enum_value<n_KeyLengthExceeded, common_status, common_status::KeyLengthExceeded>,
        wjson::enum_value<n_ValueLengthExceeded, common_status, common_status::ValueLengthExceeded>,
        wjson::enum_value<n_PrefixLengthExceeded, common_status, common_status::PrefixLengthExceeded>,
        wjson::enum_value<n_RangeLimitExceeded, common_status, common_status::RangeLimitExceeded>,
        wjson::enum_value<n_InvalidSeqNumber, common_status, common_status::InvalidSeqNumber>,
        wjson::enum_value<n_CompactFail, common_status, common_status::CompactFail>
    >
  > meta;

  typedef meta::target target;
  typedef meta::serializer serializer;
  typedef meta::member_list member_list;
};

}}

namespace std{
  inline std::ostream& operator<< (std::ostream& os, wamba::prefixdb::common_status cs)
  {
    std::string status;
    wamba::prefixdb::common_status_json::serializer()(cs, std::back_inserter(status) );
    if (!status.empty() && status[0]=='"')
      status.erase(status.begin());
    if (!status.empty() && *status.rbegin()=='"')
      status.pop_back();

    os << status;
    return os;
  }
}
