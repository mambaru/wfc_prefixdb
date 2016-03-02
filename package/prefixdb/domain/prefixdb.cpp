#include "prefixdb.hpp"
#include <wfc/logger.hpp>
#include <wfc/core/icore.hpp>
#include <wfc/module/iinstance.hpp>
#include "storage/multidb.hpp"
#include <ctime>

namespace wamba{ namespace prefixdb {
  
class prefixdb::impl: public multidb
{};

class timer
  : public std::enable_shared_from_this< timer >
{
  struct options
  {
    std::string start_date;
    time_t start_wait_ms;
    time_t interval_ms;
  };
  
  typedef ::iow::asio::deadline_timer deadline_timer;
  typedef std::unique_ptr<deadline_timer> timer_ptr; 

  timer( ::iow::asio::io_service& io, const options& opt )
    : _opt(opt)
  {
    if ( opt.interval_ms == 0 )
      return;
    
    _timer = std::make_unique<deadline_timer>(io);
  }

  void start(std::function<void()> fun)
  {
    this->wait_( _opt.start_wait_ms, fun );
    /*
    _timer->expires_from_now( ::boost::posix_time::seconds( _opt.start_wait_s ) );
    std::weak_ptr<timer> wthis = this->shared_from_this();
    _timer->async_wait( [wthis, fun](const boost::system::error_code& ec){
      if ( !ec )
      {
        // TODO: LOG
        return;
      }
      if (auto pthis = wthis.lock() )
      {
        pthis->handler(fun);
      }
    });
    */
  }
  
private:
  void wait_( time_t ms, std::function<void()> fun )
  {
    _timer->expires_from_now( ::boost::posix_time::microseconds( ms ) );
    std::weak_ptr<timer> wthis = this->shared_from_this();
    _timer->async_wait( [wthis, fun](const boost::system::error_code& ec){
      if ( !ec )
      {
        // TODO: LOG
        return;
      }
      if (auto pthis = wthis.lock() )
      {
        pthis->handler(fun);
      }
    });
    
  }
  
  void handler( std::function<void()> fun )
  {
    try{
      fun();
    }catch(...){
      // TODO: LOG
    };
    this->wait_(_opt.interval_ms, fun);
    /*
    _timer->expires_from_now( ::boost::posix_time::microseconds( _opt.interval_ms ) );
    std::weak_ptr<timer> wthis = this->shared_from_this();
    _timer->async_wait( [wthis, fun](){
      if (auto pthis = wthis.lock() )
      {
        pthis->handler(fun);
      }
    });
    */
  }
  
private:
  
  options _opt;
  timer_ptr _timer;
};

template<typename Fun>
void prefixdb::deadline_(time_t period, timer_ptr& timer, Fun dfun, void (prefixdb::* ifun)() )
{
  if ( period == 0 )
    return;
  
  if ( timer == nullptr )
  {
    timer = std::make_unique<deadline_timer>(
      this->global()->io_service,  
      boost::posix_time::seconds( period ) 
    );
  }
  else
  {
    
    //timer->expires_at()
    dfun();
    //timer->expires_at( timer->expires_from_now() + ::boost::posix_time::seconds( period ) );
    timer->expires_from_now( ::boost::posix_time::seconds( period ) );
  }
  
  std::weak_ptr<prefixdb> wthis = this->shared_from_this();
  timer->async_wait([wthis, ifun](const boost::system::error_code& )
  {
    if ( auto pthis = wthis.lock() )
    {
      prefixdb* p = pthis.get();
      (p->*ifun)();
    }
  });
}

void prefixdb::do_backup_()
{
  std::weak_ptr<impl> wimpl = _impl;
  bool compact = this->options().compact_before_backup;
  auto backup = [wimpl, compact](){
    if ( auto pimpl = wimpl.lock() )
      pimpl->backup(compact);
  };

  this->deadline_(
    this->options().backup_period_s,
    this->_backup_timer,
    backup,
    &prefixdb::do_backup_
  );
}

void prefixdb::do_restore_()
{
  std::weak_ptr<impl> wimpl = _impl;
  auto restore = [wimpl](){
    if ( auto pimpl = wimpl.lock() )
      pimpl->restore();
  };
  this->deadline_(
    this->options().restore_period_s,
    this->_restore_timer,
    restore,
    &prefixdb::do_restore_
  );
}

void prefixdb::reconfigure()
{
  auto opt = this->options();
  if ( _impl == nullptr )
  {
    _impl = std::make_shared<impl>();
    _impl->reconfigure( opt );
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
    
    if ( !_impl->reconfigure( opt ) )
    {
      wfc_abort("prefixdb open DB abort!");
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
  
  _backup_timer = nullptr;
  this->do_backup_();

  _restore_timer = nullptr;
  this->do_restore_();
}

void prefixdb::stop(const std::string&) 
{
  if ( _impl )
    _impl->close();
  
  DEBUG_LOG_BEGIN("stop prefixdb timers")
  if ( _backup_timer )
  {
    _backup_timer->cancel();
    _backup_timer->wait();
  }
  
  if (_restore_timer)
  {
    _restore_timer->cancel();
    _restore_timer->cancel();
  }
  DEBUG_LOG_END("stop prefixdb timers")
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

void prefixdb::backup( request::backup::ptr req, response::backup::handler cb)
{
  _impl->backup( std::move(req), std::move(cb) );
}

void prefixdb::restore( request::restore::ptr req, response::restore::handler cb)
{
  _impl->restore( std::move(req), std::move(cb) );
}

void prefixdb::get_updates_since( request::get_updates_since::ptr req, response::get_updates_since::handler cb)
{
  _impl->get_updates_since( std::move(req), std::move(cb) );  
}

}}
