#pragma once

#include <prefixdb/prefixdb/prefixdb_config.hpp>
#include <prefixdb/prefixdb/multidb/options/multidb_config_json.hpp>
#include <wfc/workflow.hpp>
#include <wjson/wjson.hpp>

namespace wamba{ namespace prefixdb{

struct prefixdb_config_json
{
  JSON_NAME(stop_list)
  JSON_NAME(delayed_write_workflow)

  typedef wjson::object<
    prefixdb_config,
    wjson::member_list<
      wjson::base< multidb_config_json >,
      wjson::member< n_stop_list, prefixdb_config, std::vector<std::string>,  &prefixdb_config::stop_list, wjson::vector_of_strings<> >,
      wjson::member< n_delayed_write_workflow, prefixdb_config, std::string,  &prefixdb_config::delayed_write_workflow>
    >,
    wjson::strict_mode
  > type;

  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list;
};

}}
