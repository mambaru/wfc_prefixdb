#include <fas/testing.hpp>
#include <wfc/module/testing_domain.hpp>

namespace {
  
UNIT(init, "")
{
  using namespace fas::testing;
  t << nothing;
}

}

BEGIN_SUITE(prefixdb_suite, "")
  ADD_UNIT( init )
END_SUITE(prefixdb_suite)
