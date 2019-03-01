#pragma once

#include <wfc/json.hpp>
#include <wfc/name.hpp>
#include <wfc/workflow.hpp>
#include <prefixdb/prefixdb/prefixdb_config.hpp>
#include <prefixdb/prefixdb/multidb/options/multidb_config_json.hpp>

namespace wamba{ namespace prefixdb{

struct prefixdb_config_json
{
  JSON_NAME(stop_list)
  JSON_NAME(delayed_write_workflow)
  
  typedef wfc::json::object<
    prefixdb_config,
    wfc::json::member_list<
      wfc::json::base< multidb_config_json >,
      wfc::json::member< n_stop_list, prefixdb_config, std::vector<std::string>,  &prefixdb_config::stop_list, wfc::json::vector_of_strings<> >,
      wfc::json::member< n_delayed_write_workflow, prefixdb_config, std::string,  &prefixdb_config::delayed_write_workflow>
    >,
    wfc::json::strict_mode
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list; 
};

}}
