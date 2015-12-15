//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/module/module.hpp>

namespace wamba{ namespace prefixdb{

class prefixdb_module
  : public ::wfc::module
{
  class impl;
public:
  prefixdb_module();
};

}}

