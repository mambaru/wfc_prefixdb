#pragma once

#include <prefixdb/iprefixdb.hpp>
#include <prefixdb/api/get_json.hpp>
#include <prefixdb/api/set_json.hpp>
#include <prefixdb/api/setnx_json.hpp>
#include <prefixdb/api/has_json.hpp>
#include <prefixdb/api/del_json.hpp>
#include <prefixdb/api/inc_json.hpp>
#include <prefixdb/api/add_json.hpp>
#include <prefixdb/api/packed_json.hpp>
#include <prefixdb/api/range_json.hpp>
#include <prefixdb/api/get_updates_since_json.hpp>
#include <prefixdb/api/get_all_prefixes_json.hpp>
#include <prefixdb/api/detach_prefixes_json.hpp>
#include <prefixdb/api/attach_prefixes_json.hpp>
#include <prefixdb/api/delay_background_json.hpp>
#include <prefixdb/api/continue_background_json.hpp>
#include <prefixdb/api/compact_prefix_json.hpp>

#include <prefixdb/api/create_snapshot_json.hpp>
#include <prefixdb/api/release_snapshot_json.hpp>

#include <wfc/jsonrpc.hpp>

namespace wamba{ namespace prefixdb{ namespace gateway{

JSONRPC_TAG(get)
JSONRPC_TAG(set)
JSONRPC_TAG(setnx)
JSONRPC_TAG(has)
JSONRPC_TAG(del)
JSONRPC_TAG(inc)
JSONRPC_TAG(add)
JSONRPC_TAG(packed)
JSONRPC_TAG(range)
JSONRPC_TAG(get_updates_since)
JSONRPC_TAG(get_all_prefixes)
JSONRPC_TAG(detach_prefixes)
JSONRPC_TAG(attach_prefixes)
JSONRPC_TAG(delay_background)
JSONRPC_TAG(continue_background)
JSONRPC_TAG(compact_prefix)
JSONRPC_TAG(create_snapshot)
JSONRPC_TAG(release_snapshot)



struct method_list: wfc::jsonrpc::method_list
<
  wfc::jsonrpc::interface_<iprefixdb>,
  wfc::jsonrpc::call_method< _get_, request::get_json, response::get_json>,
  wfc::jsonrpc::call_method< _set_, request::set_json, response::set_json>,
  wfc::jsonrpc::call_method< _setnx_, request::setnx_json, response::setnx_json>,
  wfc::jsonrpc::call_method< _has_, request::has_json, response::has_json>,
  wfc::jsonrpc::call_method< _del_, request::del_json, response::del_json>,
  wfc::jsonrpc::call_method< _inc_, request::inc_json, response::inc_json>,
  wfc::jsonrpc::call_method< _add_, request::add_json, response::add_json>,
  wfc::jsonrpc::call_method< _packed_, request::packed_json, response::packed_json>,
  wfc::jsonrpc::call_method< _range_, request::range_json, response::range_json>,
  wfc::jsonrpc::call_method< _get_updates_since_, request::get_updates_since_json, response::get_updates_since_json>,
  wfc::jsonrpc::call_method< _get_all_prefixes_, request::get_all_prefixes_json, response::get_all_prefixes_json>,
  wfc::jsonrpc::call_method< _detach_prefixes_, request::detach_prefixes_json, response::detach_prefixes_json>,
  wfc::jsonrpc::call_method< _attach_prefixes_, request::attach_prefixes_json, response::attach_prefixes_json>,
  wfc::jsonrpc::call_method< _delay_background_, request::delay_background_json, response::delay_background_json>,
  wfc::jsonrpc::call_method< _continue_background_, request::continue_background_json, response::continue_background_json>,
  wfc::jsonrpc::call_method< _compact_prefix_, request::compact_prefix_json, response::compact_prefix_json>,
  wfc::jsonrpc::call_method< _create_snapshot_, request::create_snapshot_json, response::create_snapshot_json>,
  wfc::jsonrpc::call_method< _release_snapshot_, request::release_snapshot_json, response::release_snapshot_json>
>
{
};

template<typename Base>
class prefixdb_interface
  : public Base
{
public:
  
  virtual void get(request::get::ptr req, response::get::handler cb ) override
  {
    this->template call< _get_ >( std::move(req), cb, nullptr);
  }

  virtual void set(request::set::ptr req, response::set::handler cb ) override
  {
    this->template call< _set_ >( std::move(req), cb, nullptr);
  }

  virtual void setnx(request::setnx::ptr req, response::setnx::handler cb ) override
  {
    this->template call< _setnx_ >( std::move(req), cb, nullptr);
  }

  virtual void has(request::has::ptr req, response::has::handler cb ) override
  {
    this->template call< _has_ >( std::move(req), cb, nullptr);
  }

  virtual void del(request::del::ptr req, response::del::handler cb ) override
  {
    this->template call< _del_ >( std::move(req), cb, nullptr);
  }

  virtual void inc(request::inc::ptr req, response::inc::handler cb ) override
  {
    this->template call< _inc_ >( std::move(req), cb, nullptr);
  }

  virtual void add(request::add::ptr req, response::add::handler cb ) override
  {
    this->template call< _add_ >( std::move(req), cb, nullptr);
  }

  virtual void packed(request::packed::ptr req, response::packed::handler cb ) override
  {
    this->template call< _packed_ >( std::move(req), cb, nullptr);
  }

  virtual void range(request::range::ptr req, response::range::handler cb ) override
  {
    this->template call< _range_ >( std::move(req), cb, nullptr);
  }

  virtual void get_updates_since(request::get_updates_since::ptr req, response::get_updates_since::handler cb ) override
  {
    this->template call< _get_updates_since_ >( std::move(req), cb, nullptr);
  }

  virtual void get_all_prefixes(request::get_all_prefixes::ptr req, response::get_all_prefixes::handler cb ) override
  {
    this->template call< _get_all_prefixes_ >( std::move(req), cb, nullptr);
  }

  virtual void detach_prefixes( request::detach_prefixes::ptr req, response::detach_prefixes::handler cb)  override
  {
    this->template call< _detach_prefixes_ >( std::move(req), cb, nullptr);
  }

  virtual void attach_prefixes( request::attach_prefixes::ptr req, response::attach_prefixes::handler cb)  override
  {
    this->template call< _attach_prefixes_ >( std::move(req), cb, nullptr);
  }

  virtual void delay_background( request::delay_background::ptr req, response::delay_background::handler cb) override
  {
    this->template call< _delay_background_ >( std::move(req), cb, nullptr);
  }
  
  virtual void continue_background( request::continue_background::ptr req, response::continue_background::handler cb) override
  {
    this->template call< _continue_background_ >( std::move(req), cb, nullptr);
  }

  virtual void compact_prefix( request::compact_prefix::ptr req, response::compact_prefix::handler cb) override
  {
    this->template call< _compact_prefix_ >( std::move(req), cb, nullptr);
  }

  virtual void create_snapshot( request::create_snapshot::ptr req, response::create_snapshot::handler cb) override
  {
    this->template call< _create_snapshot_ >( std::move(req), cb, nullptr);
  }

  virtual void release_snapshot( request::release_snapshot::ptr req, response::release_snapshot::handler cb) override
  {
    this->template call< _release_snapshot_ >( std::move(req), cb, nullptr);
  }

  virtual void reg_io( ::wfc::iinterface::io_id_t /*io_id*/, std::weak_ptr< ::wfc::iinterface> /*itf*/) override
  {
    
  }

  virtual void unreg_io( ::wfc::iinterface::io_id_t /*io_id*/) override
  {
    
  }
};
  
}}}
