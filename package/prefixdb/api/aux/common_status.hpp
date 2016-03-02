#pragma once

namespace wamba { namespace prefixdb {

enum class common_status
{
  OK,
  TransactLogError,
  EmptyFields,
  EmptyPrefix, 
  CreatePrefixFail,
  PrefixNotFound,
  WriteError,
  SomeFieldFail, // Несоответсвие типа по некоторым (или всех) полям
  TooManyKeys,     // Слишком много ключей в запросе 
  KeyLengthExceeded,
  ValueLengthExceeded,
  PrefixLengthExceeded
};

}}
