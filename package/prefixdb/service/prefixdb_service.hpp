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


#include <wfc/jsonrpc.hpp>

namespace wamba{ namespace prefixdb{ namespace service{

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
JSONRPC_TAG(continue_background)
JSONRPC_TAG(delay_background)



struct method_list: wfc::jsonrpc::method_list
<
  wfc::jsonrpc::target<iprefixdb>,
  wfc::jsonrpc::invoke_method< _get_, request::get_json,  response::get_json, iprefixdb, &iprefixdb::get>,
  wfc::jsonrpc::invoke_method< _set_, request::set_json,  response::set_json, iprefixdb, &iprefixdb::set>,
  wfc::jsonrpc::invoke_method< _has_, request::has_json,  response::has_json, iprefixdb, &iprefixdb::has>,
  wfc::jsonrpc::invoke_method< _del_, request::del_json,  response::del_json, iprefixdb, &iprefixdb::del>,
  wfc::jsonrpc::invoke_method< _inc_, request::inc_json,  response::inc_json, iprefixdb, &iprefixdb::inc>,
  wfc::jsonrpc::invoke_method< _add_, request::add_json,  response::add_json, iprefixdb, &iprefixdb::add>,
  wfc::jsonrpc::invoke_method< _setnx_, request::setnx_json,  response::setnx_json, iprefixdb, &iprefixdb::setnx>,
  wfc::jsonrpc::invoke_method< _packed_, request::packed_json,  response::packed_json, iprefixdb, &iprefixdb::packed>,
  wfc::jsonrpc::invoke_method< _range_, request::range_json,  response::range_json, iprefixdb, &iprefixdb::range>,
  wfc::jsonrpc::invoke_method< _get_updates_since_, request::get_updates_since_json,  response::get_updates_since_json, iprefixdb, &iprefixdb::get_updates_since>,
  wfc::jsonrpc::invoke_method< _get_all_prefixes_, request::get_all_prefixes_json,  response::get_all_prefixes_json, iprefixdb, &iprefixdb::get_all_prefixes>,
  wfc::jsonrpc::invoke_method< _detach_prefixes_, request::detach_prefixes_json,  response::detach_prefixes_json, iprefixdb, &iprefixdb::detach_prefixes>,
  wfc::jsonrpc::invoke_method< _attach_prefixes_, request::attach_prefixes_json,  response::attach_prefixes_json, iprefixdb, &iprefixdb::attach_prefixes>,
  wfc::jsonrpc::invoke_method< _delay_background_, request::delay_background_json,  response::delay_background_json, iprefixdb, &iprefixdb::delay_background>,
  wfc::jsonrpc::invoke_method< _continue_background_, request::continue_background_json,  response::continue_background_json, iprefixdb, &iprefixdb::continue_background>
>
{
};

}}}
