#pragma once

namespace wamba { namespace prefixdb {

enum class common_status
{
  OK,
  PrefixNotFound,
  SomeFieldFail // Несоответсвие типа по некоторым (или всех) полям
};

}}
