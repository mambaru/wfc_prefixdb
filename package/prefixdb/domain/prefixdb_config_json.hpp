#pragma once

#include <wfc/json.hpp>
#include <wfc/name.hpp>
#include <wfc/workflow.hpp>
#include <prefixdb/domain/prefixdb_config.hpp>
#include <prefixdb/domain/storage/multidb_config_json.hpp>

namespace wamba{ namespace prefixdb{

struct prefixdb_config_json
{
  JSON_NAME(stop_list)
  JSON_NAME(backup_time)
  JSON_NAME(backup_period_s)
  JSON_NAME(archive_time)
  JSON_NAME(archive_period_s)
  
  JSON_NAME(workflow)
  
  typedef wfc::json::object<
    prefixdb_config,
    ::wfc::json::member_list<
      ::wfc::json::base<multidb_config_json>,
      ::wfc::json::member<n_workflow, prefixdb_config, ::wfc::workflow_options, &prefixdb_config::workflow_opt, ::wfc::workflow_options_json>,
      ::wfc::json::member<n_stop_list, prefixdb_config, std::vector<std::string>,  &prefixdb_config::stop_list,
                          ::wfc::json::array< std::vector< ::wfc::json::value<std::string> > >
      >
    >
  > type;
  
  typedef type::target target;
  typedef type::serializer serializer;
  typedef type::member_list member_list; 
};

}}
