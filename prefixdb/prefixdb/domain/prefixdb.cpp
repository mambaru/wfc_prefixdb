#include "prefixdb.hpp"
#include <wfc/logger.hpp>
namespace wamba{ namespace prefixdb {

void prefixdb::reconfigure()
{
}

void prefixdb::set( request::set::ptr req, response::set::handler cb)
{
  DOMAIN_LOG_FATAL("prefixdb::set not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
}

void prefixdb::get( request::get::ptr req, response::get::handler cb)
{ 
  DOMAIN_LOG_FATAL("prefixdb::get not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
}

}}
