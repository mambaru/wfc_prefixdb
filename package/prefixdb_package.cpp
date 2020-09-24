//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#include "prefixdb_build_info.h"
#include "prefixdb_package.hpp"
#include "prefixdb/prefixdb_module.hpp"
#include <wfc/module/module_list.hpp>
#include <wfc/name.hpp>

namespace wamba{ namespace prefixdb{

namespace
{
  class impl: public ::wfc::module_list<
    prefixdb_build_info,
    prefixdb_module
  >
  {
  public:
    virtual std::string description() const override
    {
      return "PrefixDB package. Included PrefixDB.";
    }
  };
}

prefixdb_package::prefixdb_package()
  : package( std::make_shared<impl>() )
{
}

}}
