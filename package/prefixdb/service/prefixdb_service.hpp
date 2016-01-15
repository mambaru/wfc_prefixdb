#pragma once

#include <prefixdb/iprefixdb.hpp>
#include <prefixdb/api/get_json.hpp>
#include <prefixdb/api/set_json.hpp>
#include <prefixdb/api/has_json.hpp>
#include <prefixdb/api/del_json.hpp>
#include <prefixdb/api/inc_json.hpp>
#include <prefixdb/api/packed_json.hpp>
#include <wfc/jsonrpc.hpp>

namespace wamba{ namespace prefixdb{ namespace service{

JSONRPC_TAG(get)
JSONRPC_TAG(set)
JSONRPC_TAG(has)
JSONRPC_TAG(del)
JSONRPC_TAG(inc)
JSONRPC_TAG(packed)

struct method_list: wfc::jsonrpc::method_list
<
  wfc::jsonrpc::target<iprefixdb>,
  wfc::jsonrpc::invoke_method< _get_, request::get_json,  response::get_json, iprefixdb, &iprefixdb::get>,
  wfc::jsonrpc::invoke_method< _set_, request::set_json,  response::set_json, iprefixdb, &iprefixdb::set>,
  wfc::jsonrpc::invoke_method< _has_, request::has_json,  response::has_json, iprefixdb, &iprefixdb::has>,
  wfc::jsonrpc::invoke_method< _del_, request::del_json,  response::del_json, iprefixdb, &iprefixdb::del>,
  wfc::jsonrpc::invoke_method< _inc_, request::inc_json,  response::inc_json, iprefixdb, &iprefixdb::inc>,
  wfc::jsonrpc::invoke_method< _packed_, request::packed_json,  response::packed_json, iprefixdb, &iprefixdb::packed>
>
{
};

}}}
