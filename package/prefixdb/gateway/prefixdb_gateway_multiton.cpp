//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#include "prefixdb_gateway_multiton.hpp"
#include "prefixdb_gateway.hpp"

#include <wfc/module/multiton.hpp>
#include <wfc/module/instance.hpp>
#include <wfc/name.hpp>

namespace wamba{ namespace prefixdb{

WFC_NAME2(prefixdb_gateway_name, "prefixdb-gateway")

class prefixdb_gateway_multiton::impl: public ::wfc::jsonrpc::gateway_multiton
< 
  prefixdb_gateway_name, 
  gateway::method_list, 
  gateway::prefixdb_interface 
> 
{
};

prefixdb_gateway_multiton::prefixdb_gateway_multiton()
  : wfc::component( std::make_shared<impl>() )
{
}

}}
