//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#include <prefixdb/domain/prefixdb_config_json.hpp>
#include <prefixdb/domain/prefixdb_multiton.hpp>
#include <prefixdb/domain/prefixdb.hpp>

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
  prefixdb_config_json, 
  int( ::wfc::component_features::SuspendSupport )
>
{  
  
};

}

prefixdb_multiton::prefixdb_multiton():
  ::wfc::component( std::make_shared<impl>() )
{
}

}}
