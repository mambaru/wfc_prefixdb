#pragma once

#include <prefixdb/iprefixdb.hpp>
#include <prefixdb/api/get_json.hpp>
#include <prefixdb/api/set_json.hpp>
#include <prefixdb/api/has_json.hpp>
#include <prefixdb/api/del_json.hpp>
#include <prefixdb/api/inc_json.hpp>
#include <prefixdb/api/add_json.hpp>
#include <prefixdb/api/packed_json.hpp>
#include <prefixdb/api/range_json.hpp>
#include <prefixdb/api/backup_json.hpp>
#include <prefixdb/api/restore_json.hpp>
#include <wfc/jsonrpc.hpp>

namespace wamba{ namespace prefixdb{ namespace service{

JSONRPC_TAG(get)
JSONRPC_TAG(set)
JSONRPC_TAG(has)
JSONRPC_TAG(del)
JSONRPC_TAG(inc)
JSONRPC_TAG(add)
JSONRPC_TAG(packed)
JSONRPC_TAG(range)
JSONRPC_TAG(backup)
JSONRPC_TAG(restore)

struct method_list: wfc::jsonrpc::method_list
<
  wfc::jsonrpc::target<iprefixdb>,
  wfc::jsonrpc::invoke_method< _get_, request::get_json,  response::get_json, iprefixdb, &iprefixdb::get>,
  wfc::jsonrpc::invoke_method< _set_, request::set_json,  response::set_json, iprefixdb, &iprefixdb::set>,
  wfc::jsonrpc::invoke_method< _has_, request::has_json,  response::has_json, iprefixdb, &iprefixdb::has>,
  wfc::jsonrpc::invoke_method< _del_, request::del_json,  response::del_json, iprefixdb, &iprefixdb::del>,
  wfc::jsonrpc::invoke_method< _inc_, request::inc_json,  response::inc_json, iprefixdb, &iprefixdb::inc>,
  wfc::jsonrpc::invoke_method< _add_, request::add_json,  response::add_json, iprefixdb, &iprefixdb::add>,
  wfc::jsonrpc::invoke_method< _packed_, request::packed_json,  response::packed_json, iprefixdb, &iprefixdb::packed>,
  wfc::jsonrpc::invoke_method< _range_, request::range_json,  response::range_json, iprefixdb, &iprefixdb::range>,
  wfc::jsonrpc::invoke_method< _backup_, request::backup_json,  response::backup_json, iprefixdb, &iprefixdb::backup>,
  wfc::jsonrpc::invoke_method< _restore_, request::restore_json,  response::restore_json, iprefixdb, &iprefixdb::restore>
>
{
};

}}}
