//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/module/component.hpp>

namespace wamba{ namespace prefixdb{

class prefixdb_gateway_multiton
  : public ::wfc::component
{
  class impl;
public:
  prefixdb_gateway_multiton();
};

}}
