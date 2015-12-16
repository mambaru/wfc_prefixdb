#pragma once

#include <prefixdb/iprefixdb.hpp>
#include <prefixdb/api/get_json.hpp>
#include <prefixdb/api/set_json.hpp>
#include <prefixdb/api/has_json.hpp>
#include <prefixdb/api/del_json.hpp>
#include <wfc/jsonrpc.hpp>

namespace wamba{ namespace prefixdb{ namespace gateway{

JSONRPC_TAG(get)
JSONRPC_TAG(set)
JSONRPC_TAG(has)
JSONRPC_TAG(del)

struct method_list: wfc::jsonrpc::method_list
<
  wfc::jsonrpc::interface_<iprefixdb>,
  wfc::jsonrpc::call_method< _get_, request::get_json, response::get_json>,
  wfc::jsonrpc::call_method< _set_, request::set_json, response::set_json>,
  wfc::jsonrpc::call_method< _has_, request::has_json, response::has_json>,
  wfc::jsonrpc::call_method< _del_, request::del_json, response::del_json>
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

  virtual void has(request::has::ptr req, response::has::handler cb ) override
  {
    this->template call< _has_ >( std::move(req), cb, nullptr);
  }

  virtual void del(request::del::ptr req, response::del::handler cb ) override
  {
    this->template call< _del_ >( std::move(req), cb, nullptr);
  }
};

}}}
