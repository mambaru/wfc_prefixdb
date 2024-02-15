//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#include "prefixdb_module.hpp"
#include "domain/prefixdb_multiton.hpp"
#include "service/prefixdb_service_multiton.hpp"
#include "gateway/prefixdb_gateway_multiton.hpp"

#include <wfc/module/component_list.hpp>
#include <wfc/name.hpp>

namespace wamba{ namespace prefixdb{

namespace {
  
  WFC_NAME2(module_name, "prefixdb")

  class impl: public ::wfc::component_list<
    module_name,
    prefixdb_service_multiton,
    prefixdb_multiton,
    prefixdb_gateway_multiton
  >
  {
  public:
    virtual std::string description() const override
    {
      return "PrefixDB module. Engine google leveldb with update packed and lua scripts. Created new DB for evry unique prefix.";
    }
  };

}

prefixdb_module::prefixdb_module()
  : module( std::make_shared<impl>() )
{
}

}}
