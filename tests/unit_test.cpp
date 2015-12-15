#include <fas/testing.hpp>

UNIT(add, "")
{
  using namespace fas::testing;
  t << nothing;
  t << nothing;
}


BEGIN_SUITE(leveldb_unit_suite, "")
  ADD_UNIT( add )
END_SUITE(leveldb_unit_suite)
