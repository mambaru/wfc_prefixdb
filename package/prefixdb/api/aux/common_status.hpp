#pragma once

namespace wamba { namespace prefixdb {

enum class common_status
{
  OK,
  EmptyFields,
  EmptyPrefix, 
  CreatePrefixFail,
  PrefixNotFound,
  TooManyKeys,     // Слишком много ключей в запросе 
  KeyLengthExceeded,
  ValueLengthExceeded,
  PrefixLengthExceeded, 
  RangeLimitExceeded, 
  InvalidSeqNumber
};

}}
