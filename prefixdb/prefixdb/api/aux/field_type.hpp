#pragma once

namespace wamba { namespace prefixdb {

enum class field_type
{
  none,         // без типа, поля нет
  string,
  number,
  package,
  any          // любой тип
};

}}
