#pragma once

#include <prefixdb/domain/prefixdb_config.hpp>
#include <prefixdb/domain/storage/multidb.hpp>
#include <prefixdb/iprefixdb.hpp>

#include <wfc/domain_object.hpp>
#include <wfc/workflow.hpp>
#include <iow/io/timer/timer.hpp>
#include <memory>

namespace wamba{ namespace prefixdb{

class prefixdb
  : public ::wfc::domain_object<iprefixdb, prefixdb_config>
  , public std::enable_shared_from_this< prefixdb >
{
  class impl;
public:
  // domain_object
  virtual void start(const std::string&) override;
  virtual void reconfigure() override;
  virtual void stop(const std::string&) override;
  
  // iprefixdb
  virtual void set( request::set::ptr req, response::set::handler cb) override;
  virtual void get( request::get::ptr req, response::get::handler cb) override;
  virtual void has( request::has::ptr req, response::has::handler cb) override;
  virtual void del( request::del::ptr req, response::del::handler cb) override;
  virtual void inc( request::inc::ptr req, response::inc::handler cb) override;
  virtual void add( request::add::ptr req, response::add::handler cb) override;
  virtual void packed( request::packed::ptr req, response::packed::handler cb) override;
  virtual void range( request::range::ptr req, response::range::handler cb) override;
  virtual void backup( request::backup::ptr req, response::backup::handler cb) override;
  virtual void restore( request::restore::ptr req, response::restore::handler cb) override;
  virtual void get_updates_since( request::get_updates_since::ptr req, response::get_updates_since::handler cb) override;

private:
  /*
  typedef boost::asio::deadline_timer deadline_timer;
  typedef std::unique_ptr<deadline_timer> timer_ptr; 
  void do_backup_();
  void do_restore_();
  template<typename Fun>
  void deadline_(time_t period, timer_ptr& timer, Fun dfun, void (prefixdb::* ifun)() );
  */

private:
  std::shared_ptr<impl> _impl;

  std::shared_ptr< ::wfc::workflow> _flow;
  ::wfc::workflow::timer_id _backup_timer  = -1;
  ::wfc::workflow::timer_id _archive_timer  = -1;
    /*
  timer_ptr _backup_timer;
  timer_ptr _restore_timer;
  typedef ::iow::io::timer timer_type;
  typedef std::shared_ptr<timer_type> timer1_ptr;
  timer1_ptr _timer;
    */
};

}}
