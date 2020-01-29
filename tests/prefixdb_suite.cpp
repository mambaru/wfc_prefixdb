#include <fas/testing.hpp>
#include <wfc/module/testing_domain.hpp>
#include <prefixdb/prefixdb/prefixdb.hpp>


namespace {
  
UNIT(init, "")
{
  using namespace fas::testing;
  auto ptest = std::make_shared<wfc::testing_domain>();
  wamba::prefixdb::prefixdb::domain_config conf;
  auto pdemo = ptest->create<wamba::prefixdb::prefixdb>(conf);
  t << nothing;
}

}

BEGIN_SUITE(prefixdb_suite, "")
  ADD_UNIT( init )
END_SUITE(prefixdb_suite)
