//
// Author: Vladimir Migashko <migashko@gmail.com>, (C) 2013-2015, 2020
//
// Copyright: See COPYING file that comes with this distribution
//

#pragma once

#include <wfc/module/component.hpp>

namespace wamba{ namespace prefixdb{

class prefixdb_multiton
  : public ::wfc::component
{
public:
  prefixdb_multiton();
  virtual std::string interface_name() const override;
  virtual std::string description() const override;
  virtual std::string help(const std::string&) const override;
};

}}
