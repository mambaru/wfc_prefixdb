#pragma once

#include <prefixdb/domain/prefixdb_config.hpp>
#include <prefixdb/domain/iprefixdb.hpp>

#include <wfc/domain_object.hpp>

namespace wamba{ namespace prefixdb{

class prefixdb
  : public ::wfc::domain_object<iprefixdb, prefixdb_config>
{
public:
  virtual void reconfigure() override;
private:

};

}}
