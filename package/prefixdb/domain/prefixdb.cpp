#include "prefixdb.hpp"
#include <wfc/wfc_exit.hpp>
#include <wfc/logger.hpp>
#include <wfc/core/icore.hpp>
#include <wfc/module/iinstance.hpp>
#include "storage/multidb.hpp"
#include "storage/god.hpp"
#include "../service/prefixdb_cmd.hpp"
#include <ctime>

namespace wamba{ namespace prefixdb {
  
void prefixdb::start(const std::string&)
{
  
  if ( this->has_arg("restore") )  
  {
    auto opt = this->options();
    if ( opt.restore.forbid )
    {
      ::wfc_exit_with_error("Restore forbidden in this configurations");
      return;
    }
    
    auto db = std::make_shared<multidb>();
    auto factory = god::create("rocksdb", this->global()->io_service );
    
    std::string path = this->get_arg("restore");
    opt.preopen = false;
    /*if ( !path.empty() )
      opt.restore_path = path;*/
    factory->initialize(opt/*, true*/);
    db->reconfigure( opt, factory );

    //COMMON_LOG_MESSAGE("Restore from " << opt.restore_path)
    if ( !db->restore() )
    {
      wfc_exit_with_error("restore fail");
      return;
    }
    db->stop();
    ::wfc_exit();
    return;
  }
  this->reconfigure();
  
}

void prefixdb::configure() 
{
  
}

void prefixdb::reconfigure()
{
  auto opt = this->options();
  
  if ( _impl == nullptr )
  {
    _impl = std::make_shared<multidb>();
    auto factory = god::create("rocksdb", this->global()->io_service );
  
    
    opt.slave.master = this->global()->registry.get<iprefixdb>( opt.slave.target );
    DEBUG_LOG_MESSAGE("opt.slave.master = " << opt.slave.target)

    if ( opt.slave.master != nullptr )
    {
      DEBUG_LOG_MESSAGE("-------------------------------------")
      DEBUG_LOG_MESSAGE("slave target '" << opt.slave.target << "' enabled " << opt.slave.pull_timeout_ms << " " << opt.path )
      DEBUG_LOG_MESSAGE("-------------------------------------")
    }
    //factory->initialize(opt, false);
    _impl->reconfigure( opt, factory );
  }
  else
  {
    auto& stop_list = opt.stop_list;
    for ( const std::string& name : stop_list )
    {
      if ( auto obj = this->global()->registry.get< ::wfc::iinstance >("instance", name) )
      {
        obj->stop("");
      }
    }
    
    auto factory = god::create("rocksdb", this->global()->io_service );
    factory->initialize(opt/*, false*/);

    if ( !_impl->reconfigure( opt, factory ) )
    {
      wfc_exit_with_error("prefixdb open DB abort!");
    }
    
    for ( const std::string& name : stop_list )
    {
      if ( auto obj = this->global()->registry.get< ::wfc::iinstance >("instance", name) )
      {
        obj->start("");
      }
    }
    
    DEBUG_LOG_MESSAGE("void prefixdb::reconfigured()!!!")
  }
 
}

void prefixdb::stop(const std::string&) 
{
  
  if ( _impl )
    _impl->stop();
}

void prefixdb::set( request::set::ptr req, response::set::handler cb)
{
  _impl->set( std::move(req), std::move(cb) );
}

void prefixdb::setnx( request::setnx::ptr req, response::setnx::handler cb)
{
  _impl->setnx( std::move(req), std::move(cb) );
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

void prefixdb::add( request::add::ptr req, response::add::handler cb) 
{
  _impl->add( std::move(req), std::move(cb) );
}

void prefixdb::packed( request::packed::ptr req, response::packed::handler cb)
{
  _impl->packed( std::move(req), std::move(cb) );
}

void prefixdb::range( request::range::ptr req, response::range::handler cb)
{
  _impl->range( std::move(req), std::move(cb) );
}

void prefixdb::get_updates_since( request::get_updates_since::ptr req, response::get_updates_since::handler cb)
{
  _impl->get_updates_since( std::move(req), std::move(cb) );  
}

void prefixdb::get_all_prefixes( request::get_all_prefixes::ptr req, response::get_all_prefixes::handler cb)
{
  _impl->get_all_prefixes( std::move(req), std::move(cb) );  
}

void prefixdb::detach_prefixes( request::detach_prefixes::ptr req, response::detach_prefixes::handler cb)
{
  _impl->detach_prefixes( std::move(req), std::move(cb) );  
}

void prefixdb::delay_background( request::delay_background::ptr req, response::delay_background::handler cb) 
{
  _impl->delay_background( std::move(req), std::move(cb) );  
}

void prefixdb::perform_io(data_ptr d, io_id_t /*io_id*/, outgoing_handler_t handler)
{
  service::prefixdb_cmd(this->shared_from_this(), std::move(d), handler);
}

}}
