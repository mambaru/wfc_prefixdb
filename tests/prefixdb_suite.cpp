#include <fas/testing.hpp>
#include <wfc/testing/testing_domain.hpp>
#include <prefixdb/prefixdb/prefixdb.hpp>
#include <wlog/init.hpp>

namespace {

struct _prefixdb_;
typedef std::shared_ptr<wamba::prefixdb::prefixdb> prefixdb_ptr;

UNIT(init, "")
{
  wlog::disable();
  using namespace fas::testing;
  auto ptest = std::make_shared<wfc::testing_domain>();
  wamba::prefixdb::prefixdb::domain_config conf;
  conf.path="./db";
  auto p = ptest->create<wamba::prefixdb::prefixdb>(conf);
  GET_REF(_prefixdb_) = ptest->create<wamba::prefixdb::prefixdb>(conf);
  GET_REF(_prefixdb_) -> start();
  t << nothing;
}

UNIT(set, "")
{
  using namespace fas::testing;
  using namespace wamba::prefixdb ;
  t << flush;
  auto db = GET_REF(_prefixdb_);
  auto s = std::make_unique< request::set>();
  s->prefix="test1";
  s->fields.push_back(std::make_pair("bad_field1", "bad_value"));
  s->fields.push_back(std::make_pair("field1", "\"value1\""));
  s->fields.push_back(std::make_pair("field2", "true"));
  s->fields.push_back(std::make_pair("field3", "null"));
  s->fields.push_back(std::make_pair("field4", "[]"));
  s->fields.push_back(std::make_pair("field5", "{}"));
  s->fields.push_back(std::make_pair("field6", "6"));
  db->set(std::move(s), nullptr);
}

UNIT(get, "")
{
  using namespace fas::testing;
  using namespace wamba::prefixdb ;
  auto db = GET_REF(_prefixdb_);
  auto g = std::make_unique< request::get>();
  g->prefix="test1";
  g->fields={"field1", "field2", "field3", "field4", "field5","field6", "bad_field1"};
  response::get::ptr gg;
  db->get(std::move(g), [&gg](response::get::ptr res) noexcept { gg = std::move(res);});
  t << equal<expect, std::string>(gg->prefix, "test1") << FAS_FL;
  t << equal<assert, size_t>(gg->fields.size(), 7) << FAS_FL;
  t << equal<expect, std::string>(gg->fields[0].first, "field1") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[0].second, "\"value1\"") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[1].first, "field2") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[1].second, "true") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[2].first, "field3") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[2].second, "null") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[3].first, "field4") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[3].second, "[]") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[4].first, "field5") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[4].second, "{}") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[5].first, "field6") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[5].second, "6") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[6].first, "bad_field1") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[6].second, "\"bad_value\"") << FAS_FL;
}

UNIT(inc, "")
{
  using namespace fas::testing;
  using namespace wamba::prefixdb ;
  using namespace wjson::literals;
  t << flush;
  auto db = GET_REF(_prefixdb_);
  auto i = std::make_unique< request::inc>();
  i->prefix="test1";
  i->fields.push_back(std::make_pair("inc1", "{'inc':1, 'val':100}"_json));
  i->fields.push_back(std::make_pair("inc2", "{'inc':-1, 'val':0}"_json));
  i->fields.push_back(std::make_pair("inc3", "{'inc':-2}"_json));
  i->fields.push_back(std::make_pair("inc4", "{'val':4}"_json));
  i->fields.push_back(std::make_pair("inc1", "{'inc':1, 'val':100}"_json));
  ///i->fields.push_back(std::make_pair("inc5", "5"_json));
  db->inc(std::move(i), nullptr);
  auto g = std::make_unique< request::get>();
  g->prefix="test1";
  g->fields={"inc1", "inc2", "inc3", "inc4", "inc5"};
  response::get::ptr gg;
  db->get(std::move(g), [&gg](response::get::ptr res) noexcept { gg = std::move(res);});
  t << equal<expect, std::string>(gg->prefix, "test1") << FAS_FL;
  t << equal<assert, size_t>(gg->fields.size(), 4) << FAS_FL;
  t << equal<expect, std::string>(gg->fields[0].first, "inc1") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[0].second, "102") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[1].first, "inc2") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[1].second, "-1") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[2].first, "inc3") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[2].second, "-2") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[3].first, "inc4") << FAS_FL;
  t << equal<expect, std::string>(gg->fields[3].second, "4") << FAS_FL;
  //t << equal<expect, std::string>(gg->fields[4].first, "inc5") << FAS_FL;
  //t << equal<expect, std::string>(gg->fields[4].second, "5") << FAS_FL;
}

UNIT(clear, "")
{
  using namespace fas::testing;
  using namespace wamba::prefixdb ;
  auto db = GET_REF(_prefixdb_);
  auto gap = std::make_unique< request::get_all_prefixes>();
  response::get_all_prefixes::ptr rgap;
  db->get_all_prefixes(std::move(gap), [&rgap]( response::get_all_prefixes::ptr res ) noexcept { rgap=std::move(res); });
  for ( const std::string& prefix : rgap->prefixes )
  {
    auto r = std::make_unique<request::range>();
    r->limit = 1000000;
    r->prefix = prefix;
    response::range::ptr rr;
    db->range(std::move(r), [&rr]( response::range::ptr res ) noexcept { rr=std::move(res); });
    auto d = std::make_unique<request::del>();
    d->prefix = prefix;
    for (auto& f: rr->fields)
      d->fields.push_back(f.first);
    t << message("CLEAR:") << " delete " << d->fields.size() << " fields from '" << prefix << "'";
    db->del(std::move(d), nullptr);
  }
  t << nothing;
}

UNIT(release, "")
{
  using namespace fas::testing;
  GET_REF(_prefixdb_) -> stop();
  GET_REF(_prefixdb_) = nullptr;
}

} // namespace

BEGIN_SUITE(prefixdb_suite, "")
  ADD_UNIT( init )
  ADD_UNIT( clear )
  ADD_UNIT( set )
  ADD_UNIT( get )
  ADD_UNIT( inc )
  ADD_UNIT( release )
  ADD_VALUE( _prefixdb_, prefixdb_ptr )
END_SUITE(prefixdb_suite)
