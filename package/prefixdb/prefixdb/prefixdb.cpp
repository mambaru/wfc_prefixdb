#include "prefixdb.hpp"
#include <wfc/wfc_exit.hpp>
#include <wfc/logger.hpp>
#include <wfc/core/icore.hpp>
#include <wfc/module/iinstance.hpp>
#include "multidb/multidb.hpp"
#include "multidb/god.hpp"

#include "../service/prefixdb_cmd.hpp"
#include "../logger.hpp"
#include <ctime>

namespace wamba{ namespace prefixdb {
  
void prefixdb::start()
{
  if ( this->has_arg("restore") )  
    return this->restore_();
  this->open_prefixdb();
  _impl->start();
}


void prefixdb::open_prefixdb()
{
  auto opt = this->options();
  opt.args.workflow = this->get_workflow();
  
  if ( _impl == nullptr )
  {
    _impl = std::make_shared<multidb>();
    auto factory = god::create("rocksdb", this->global()->io_service );

    opt.slave.master = this->global()->registry.get<iprefixdb>( opt.slave.target );
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
    factory->initialize(opt);

    if ( !_impl->reconfigure( opt, factory ) )
    {
      PREFIXDB_LOG_FATAL("prefixdb open DB abort!");
    }
    
    for ( const std::string& name : stop_list )
    {
      if ( auto obj = this->global()->registry.get< ::wfc::iinstance >("instance", name) )
      {
        obj->start("");
      }
    }
  }
}


void prefixdb::stop() 
{
  if ( _impl )
    _impl->stop();
}

void prefixdb::set( request::set::ptr req, response::set::handler cb)
{
  if ( this->bad_request(req, cb) )
    return;
  
  _impl->set( std::move(req), std::move(cb) );
}

void prefixdb::setnx( request::setnx::ptr req, response::setnx::handler cb)
{
  if ( this->bad_request(req, cb) )
    return;

  _impl->setnx( std::move(req), std::move(cb) );
}

void prefixdb::get( request::get::ptr req, response::get::handler cb)
{
  if ( this->notify_ban(req, cb) )
    return;

  _impl->get( std::move(req), std::move(cb) );
}

void prefixdb::has( request::has::ptr req, response::has::handler cb)
{
  if ( this->notify_ban(req, cb) )
    return;

  _impl->has( std::move(req), std::move(cb) );
}

void prefixdb::del( request::del::ptr req, response::del::handler cb) 
{
  if ( this->bad_request(req, cb) )
    return;

  _impl->del( std::move(req), std::move(cb) );
}

void prefixdb::inc( request::inc::ptr req, response::inc::handler cb) 
{
  if ( this->bad_request(req, cb) )
    return;

  _impl->inc( std::move(req), std::move(cb) );
}

void prefixdb::add( request::add::ptr req, response::add::handler cb) 
{
  if ( this->bad_request(req, cb) )
    return;

  _impl->add( std::move(req), std::move(cb) );
}

void prefixdb::packed( request::packed::ptr req, response::packed::handler cb)
{
  if ( this->bad_request(req, cb) )
    return;

  _impl->packed( std::move(req), std::move(cb) );
}

void prefixdb::range( request::range::ptr req, response::range::handler cb)
{
  if ( this->notify_ban(req, cb) )
    return;

  _impl->range( std::move(req), std::move(cb) );
}

void prefixdb::get_updates_since( request::get_updates_since::ptr req, response::get_updates_since::handler cb)
{
  if ( this->notify_ban(req, cb) )
    return;

  _impl->get_updates_since( std::move(req), std::move(cb) );  
}

void prefixdb::get_all_prefixes( request::get_all_prefixes::ptr req, response::get_all_prefixes::handler cb)
{
  if ( this->notify_ban(req, cb) )
    return;

  _impl->get_all_prefixes( std::move(req), std::move(cb) );  
}

void prefixdb::detach_prefixes( request::detach_prefixes::ptr req, response::detach_prefixes::handler cb)
{
  if ( this->bad_request(req, cb) )
    return;

  _impl->detach_prefixes( std::move(req), std::move(cb) );  
}

void prefixdb::attach_prefixes( request::attach_prefixes::ptr req, response::attach_prefixes::handler cb)
{
  if ( this->bad_request(req, cb) )
    return;

  _impl->attach_prefixes( std::move(req), std::move(cb) );  
}

void prefixdb::delay_background( request::delay_background::ptr req, response::delay_background::handler cb) 
{
  if ( this->bad_request(req, cb) )
    return;

  _impl->delay_background( std::move(req), std::move(cb) );  
}

void prefixdb::continue_background( request::continue_background::ptr req, response::continue_background::handler cb) 
{
  if ( this->bad_request(req, cb) )
    return;

  _impl->continue_background( std::move(req), std::move(cb) );  
}

void prefixdb::compact_prefix( request::compact_prefix::ptr req, response::compact_prefix::handler cb) 
{
  if ( this->bad_request(req, cb) )
    return;
  
  _impl->compact_prefix( std::move(req), std::move(cb) );  
}

void prefixdb::perform_io(data_ptr d, io_id_t /*io_id*/, output_handler_t handler)
{
  service::prefixdb_cmd(this->shared_from_this(), std::move(d), handler);
}

void prefixdb::restore_()
{
  auto opt = this->options();
  if ( opt.restore.forbid )
  {
    PREFIXDB_LOG_FATAL("Restore forbidden in this configurations");
    return;
  }
  
  auto db = std::make_shared<multidb>();
  auto factory = god::create("rocksdb", this->global()->io_service );
  
  std::string path = this->get_arg("restore");
  if ( !path.empty() )
    opt.restore.path = path;
  if ( this->has_arg("bid") )
    opt.restore.backup_id = this->get_arg_t<uint64_t>("bid");
  opt.preopen = false;
  factory->initialize(opt);
  db->reconfigure( opt, factory );
  
  if ( !db->restore() )
  {
    wfc_exit_with_error("restore fail");
    return;
  }
  db->stop();
  ::wfc_exit();
  return; 
}

}}
