//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#include <prefixdb/prefixdb/prefixdb_config_json.hpp>
#include <prefixdb/prefixdb/prefixdb_multiton.hpp>
#include <prefixdb/prefixdb/prefixdb.hpp>

#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/module/component.hpp>
#include <wfc/name.hpp>

namespace wamba{ namespace prefixdb{

namespace {
  
WFC_NAME2(component_name, "prefixdb")

class impl: public ::wfc::multiton<
  component_name,
  ::wfc::instance<prefixdb>,
  prefixdb_config_json
>
{  
  
};

}

prefixdb_multiton::prefixdb_multiton():
  ::wfc::component( std::make_shared<impl>() )
{
}

std::string prefixdb_multiton::description() const 
{
  return "Allowed instance params:\n"
            "\t\t\t\trepair[=0|1] - repair and start\n"
            "\t\t\t\trestore[=<<path>>][:bid=<<backup id>>] - restore from backup\n"
            "\t\t\t\tload[=<<items-per-request>>] - load DB from master (for slave before start)";
}

}}
