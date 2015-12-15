#include <fas/testing.hpp>

UNIT(get, "")
{
  using namespace fas::testing;
  t << nothing;
}

BEGIN_SUITE(leveldb_perf_suite, "")
  ADD_UNIT( get )
END_SUITE(leveldb_perf_suite)
