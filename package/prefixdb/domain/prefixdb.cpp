#include "prefixdb.hpp"
#include <wfc/wfc_exit.hpp>
#include <wfc/logger.hpp>
#include <wfc/core/icore.hpp>
#include <wfc/module/iinstance.hpp>
#include "storage/multidb.hpp"
#include "storage/god.hpp"
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
    if ( !path.empty() )
      opt.restore_path = path;
    factory->initialize(opt, true);
    db->reconfigure( opt, factory );

    COMMON_LOG_MESSAGE("Restore from " << opt.restore_path)
    if ( !db->restore() )
    {
      wfc_exit_with_error("restore fail");
      return;
    }
    db->close();
    ::wfc_exit();
    return;
  }
  this->reconfigure();
  _flow->start();
}

void prefixdb::configure() 
{
  
}

void prefixdb::reconfigure()
{
  auto opt = this->options();
  if ( _flow == nullptr )
  {
    _flow = ::wfc::workflow::create(opt.workflow_opt);
  }
  
  
  _flow->reconfigure( opt.workflow_opt );
  
  if ( _backup_timer != -1 ) _flow->release_timer(_backup_timer);
  if ( opt.backup.enabled &&  !opt.backup.path.empty() )
  {
    std::weak_ptr<prefixdb> wthis = this->shared_from_this();
    _backup_timer = _flow->create_timer(
      opt.backup.start_time,
      std::chrono::seconds( opt.backup.period_s ),
      [wthis]() {
        if (auto pthis = wthis.lock() )
        {
          pthis->_impl->backup();
          return true;
        }
        return false;
      }
    );
  }
  
  if ( _archive_timer != -1 ) _flow->release_timer(_archive_timer);
  if ( opt.archive.enabled && !opt.archive.path.empty() )
  {
    std::weak_ptr<prefixdb> wthis = this->shared_from_this();
    _archive_timer = _flow->create_timer(
      opt.archive.start_time,
      std::chrono::seconds( opt.archive.period_s ),
      [wthis]() {
        DEBUG_LOG_MESSAGE("---- ARCHIVE ----")
        if (auto pthis = wthis.lock() )
        {
          pthis->_impl->archive( pthis->options().archive.path );
          return true;
        }
        return false;
      }
    );
  }
  if ( _impl == nullptr )
  {
    _impl = std::make_shared<multidb>();
    auto factory = god::create("rocksdb", this->global()->io_service );
  
    opt.slave.timer = _flow;
    opt.slave.master = this->global()->registry.get<iprefixdb>( opt.slave.target );
    DEBUG_LOG_MESSAGE("opt.slave.master = " << opt.slave.target)

    if ( opt.slave.master != nullptr )
    {
      DEBUG_LOG_MESSAGE("-------------------------------------")
      DEBUG_LOG_MESSAGE("slave target '" << opt.slave.target << "' enabled " << opt.slave.pull_timeout_ms << " " << opt.path )
      DEBUG_LOG_MESSAGE("-------------------------------------")
    }
    factory->initialize(opt, false);
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
    factory->initialize(opt, false);

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
  if ( _flow )
    _flow->stop();
  
  if ( _impl )
    _impl->close();
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

}}
