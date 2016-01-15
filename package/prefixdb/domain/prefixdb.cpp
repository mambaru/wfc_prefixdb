#include "prefixdb.hpp"
#include <wfc/logger.hpp>
#include <wfc/core/icore.hpp>
#include "storage/multidb.hpp"

namespace wamba{ namespace prefixdb {
  
class prefixdb::impl: public multidb
{};

void prefixdb::reconfigure()
{
  if ( _impl == nullptr )
    _impl = std::make_shared<impl>();
  if ( !_impl->reconfigure( this->options() ) )
  {
    wfc_abort("prefixdb open DB abort!");
    //this->global()->registry.get< ::wfc::icore>("core")->core_stop();
  }
}

void prefixdb::set( request::set::ptr req, response::set::handler cb)
{
  _impl->set( std::move(req), std::move(cb) );
}

void prefixdb::get( request::get::ptr req, response::get::handler cb)
{
  _impl->get( std::move(req), std::move(cb) );
}

void prefixdb::has( request::has::ptr req, response::has::handler cb)
{
  _impl->has( std::move(req), std::move(cb) );
}

void prefixdb::del( request::del::ptr req, response::del::handler cb) 
{
  _impl->del( std::move(req), std::move(cb) );
}

void prefixdb::inc( request::inc::ptr req, response::inc::handler cb) 
{
  _impl->inc( std::move(req), std::move(cb) );
}

void prefixdb::upd( request::upd::ptr req, response::upd::handler cb) 
{
  _impl->upd( std::move(req), std::move(cb) );
}

void prefixdb::packed( request::packed::ptr req, response::packed::handler cb)
{
  _impl->packed( std::move(req), std::move(cb) );
}

}}
