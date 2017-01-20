#pragma once

#include <prefixdb/prefixdb/prefixdb_config.hpp>
#include <prefixdb/iprefixdb.hpp>

#include <wfc/domain_object.hpp>
#include <wfc/workflow.hpp>
#include <iow/io/timer/timer.hpp>
#include <memory>

namespace wamba{ namespace prefixdb{

class multidb;

class prefixdb
  : public ::wfc::domain_object<iprefixdb, prefixdb_config, ::wfc::nostat>
  , public std::enable_shared_from_this< prefixdb >
{
public:
  // domain_object
  virtual void start() override;
  virtual void initialize() override;
  virtual void stop() override;
  
  // iprefixdb
  virtual void set( request::set::ptr req, response::set::handler cb) override;
  virtual void get( request::get::ptr req, response::get::handler cb) override;
  virtual void has( request::has::ptr req, response::has::handler cb) override;
  virtual void del( request::del::ptr req, response::del::handler cb) override;
  virtual void inc( request::inc::ptr req, response::inc::handler cb) override;
  virtual void add( request::add::ptr req, response::add::handler cb) override;
  virtual void setnx( request::setnx::ptr req, response::setnx::handler cb) override;
  virtual void packed( request::packed::ptr req, response::packed::handler cb) override;
  virtual void range( request::range::ptr req, response::range::handler cb) override;
  virtual void get_updates_since( request::get_updates_since::ptr req, response::get_updates_since::handler cb) override;
  virtual void get_all_prefixes( request::get_all_prefixes::ptr req, response::get_all_prefixes::handler cb) override;
  virtual void detach_prefixes( request::detach_prefixes::ptr req, response::detach_prefixes::handler cb) override;
  virtual void attach_prefixes( request::attach_prefixes::ptr req, response::attach_prefixes::handler cb) override;
  virtual void delay_background( request::delay_background::ptr req, response::delay_background::handler cb) override;
  virtual void continue_background( request::continue_background::ptr req, response::continue_background::handler cb) override;
  
  // iinterface
  virtual void perform_io(data_ptr d, io_id_t /*io_id*/, outgoing_handler_t handler) override;
private:
  void restore_();
  std::shared_ptr<multidb> _impl;
};

}}
