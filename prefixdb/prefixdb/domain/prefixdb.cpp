#include "prefixdb.hpp"
#include <wfc/logger.hpp>
namespace wamba{ namespace prefixdb {

void prefixdb::reconfigure()
{
}

void prefixdb::set( request::set::ptr req, response::set::handler cb)
{
  DOMAIN_LOG_FATAL("prefixdb::set not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

void prefixdb::get( request::get::ptr req, response::get::handler cb)
{
  DOMAIN_LOG_FATAL("prefixdb::get not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

void prefixdb::has( request::has::ptr req, response::has::handler cb)
{
  DOMAIN_LOG_FATAL("prefixdb::has not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

void prefixdb::del( request::del::ptr req, response::del::handler cb) 
{
  DOMAIN_LOG_FATAL("prefixdb::del not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

void prefixdb::inc( request::inc::ptr req, response::inc::handler cb) 
{
  DOMAIN_LOG_FATAL("prefixdb::inc not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

void prefixdb::upd( request::upd::ptr req, response::upd::handler cb) 
{
  DOMAIN_LOG_FATAL("prefixdb::upd not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

}}
