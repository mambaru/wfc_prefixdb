//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#include <prefixdb/prefixdb_module.hpp>
//#include "leveldb_domain_multiton.hpp"
//#include "leveldb_service_multiton.hpp"
//#include "leveldb_gateway_multiton.hpp"
#include <prefixdb/domain/prefixdb_multiton.hpp>

#include <wfc/module/component_list.hpp>
#include <wfc/name.hpp>

namespace wamba{ namespace prefixdb{

WFC_NAME2(prefixdb_module_name, "prefixdb")

class prefixdb_module::impl: public ::wfc::component_list<
  prefixdb_module_name,
  prefixdb_multiton
  /*,
  leveldb_domain_multiton,
  leveldb_service_multiton,
  leveldb_gateway_multiton
  */
>
{
public:
  virtual std::string description() const override
  {
    return "PrefixDB module. Engine google leveldb with update packed and lua scripts. Created new DB for evry unique prefix.";
  }
};

prefixdb_module::prefixdb_module()
  : module( std::make_shared<prefixdb_module::impl>() )
{
}

}}
