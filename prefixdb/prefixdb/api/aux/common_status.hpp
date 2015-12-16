#pragma once

namespace wamba { namespace prefixdb {

enum class common_status
{
  OK,
  SomeFieldFail // Несоответсвие типа по некоторым (или всех) полям
};

}}
