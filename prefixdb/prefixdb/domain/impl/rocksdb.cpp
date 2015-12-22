#include "rocksdb.hpp"
#include <wfc/logger.hpp>

namespace wamba{ namespace prefixdb {
  
rocksdb::rocksdb( db_type* db)
  : _db(db)
{}

void rocksdb::set( request::set::ptr req, response::set::handler cb)
{
  DOMAIN_LOG_FATAL("rocksdb::set not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

void rocksdb::get( request::get::ptr req, response::get::handler cb)
{
  DOMAIN_LOG_FATAL("rocksdb::get not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

void rocksdb::has( request::has::ptr req, response::has::handler cb)
{
  DOMAIN_LOG_FATAL("rocksdb::has not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}


void rocksdb::del( request::del::ptr req, response::del::handler cb) 
{

  DOMAIN_LOG_FATAL("rocksdb::del not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

void rocksdb::inc( request::inc::ptr req, response::inc::handler cb) 
{
  DOMAIN_LOG_FATAL("rocksdb::inc not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

void rocksdb::upd( request::upd::ptr req, response::upd::handler cb) 
{
  DOMAIN_LOG_FATAL("rocksdb::upd not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

}}
