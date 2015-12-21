#include "multidb.hpp"
#include <wfc/logger.hpp>

namespace wamba{ namespace prefixdb {

void multidb::set( request::set::ptr req, response::set::handler cb)
{
  DOMAIN_LOG_FATAL("multidb::set not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

void multidb::get( request::get::ptr req, response::get::handler cb)
{
  DOMAIN_LOG_FATAL("multidb::get not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

void multidb::has( request::has::ptr req, response::has::handler cb)
{
  DOMAIN_LOG_FATAL("multidb::has not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}


void multidb::del( request::del::ptr req, response::del::handler cb) 
{

  DOMAIN_LOG_FATAL("multidb::del not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

void multidb::inc( request::inc::ptr req, response::inc::handler cb) 
{
  DOMAIN_LOG_FATAL("multidb::inc not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

void multidb::upd( request::upd::ptr req, response::upd::handler cb) 
{
  DOMAIN_LOG_FATAL("multidb::upd not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

}}
