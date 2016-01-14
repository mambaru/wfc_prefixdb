
#include <rocksdb/merge_operator.h>
#include <rocksdb/env.h>
#include "../persistent_value.hpp"
#include "merge_operator.hpp"
#include "operation_inc.hpp"
#include "operation_set.hpp"
#include "operation_upd.hpp"

#include <wfc/logger.hpp>
#include <wfc/json.hpp>
#include <prefixdb/api/upd.hpp>
#include <prefixdb/api/upd_json.hpp>

#include <algorithm>

namespace wamba{ namespace prefixdb{

  
bool merge_operator::Merge(const slice_type& key,
                     const slice_type* existing_value,
                     const slice_type& value,
                     std::string* new_value,
                     ::rocksdb::Logger* logger) const 
{
  std::string skey = key.ToString();
  std::cout << key.ToString() << std::endl;
  DEBUG_LOG_MESSAGE("merge_operator::Merge: " << key.ToString()  )
  operation op = *( reinterpret_cast<const operation*>(value.data()) );
  if ( op == operation::set)
  {
    return this->set_(key, existing_value, value, new_value, logger);
  }
  else if ( op == operation::inc)
  {
    return this->inc_(key, existing_value, value, new_value, logger);
  }
  else if ( op == operation::upd)
  {
    return this->upd_(key, existing_value, value, new_value, logger);
  }
  else
  {
    DOMAIN_LOG_ERROR("merge_operator error: unknown operation " << int(op) )
    ::rocksdb::Error(logger, "merge_operator error: unknown operation");
  }
  return false;
}
  
const char* merge_operator::Name() const 
{
  return "PreffixDBMergeOperator";
}

bool merge_operator::set_(const slice_type& key,
                          const slice_type* existing_slice,
                          const slice_type& value_op,
                          std::string* new_value,
                          ::rocksdb::Logger* /*logger*/) const
{
  operation_set op;
  operation_set::deserialize<slice_type>(value_op, op);
  
  if ( existing_slice )
  {
    persistent_value val = persistent_value::deserialize<slice_type>( *existing_slice, false );
    if ( !op.force && val.type!=op.type && val.type!=field_type::any )
    {
      COMMON_LOG_WARNING("prefixdb updater 'set' for key=" << key.ToString() << " bad type. Not changed!")
      new_value->assign( existing_slice->data(), existing_slice->size() );
      // persistent_value::serialize<slice_type>(*new_value, val.data, val.type, val.ttl);
      return true;
    }
  }
  persistent_value::serialize<slice_type>(*new_value, op.val, op.type, op.ttl);
  return true;
}

bool merge_operator::inc_(const slice_type& key,
                          const slice_type* existing_slice,
                          const slice_type& value_op,
                          std::string* new_value,
                          ::rocksdb::Logger* /*logger*/) const
{
  operation_inc op;
  operation_inc::deserialize<slice_type>(value_op, op);
  int64_t ival = op.def;
  if ( existing_slice )
  {
    persistent_value val = persistent_value::deserialize<slice_type>( *existing_slice, false );
    if ( !op.force && val.type!=field_type::number )
    {
      COMMON_LOG_WARNING("prefixdb updater 'inc' for key=" << key.ToString() << " bad type. Not changed!")
      new_value->assign( existing_slice->data(), existing_slice->size() );
      // persistent_value::serialize<slice_type>(*new_value, val.data, val.type, val.ttl);
      return true;
    }
    try{ ival = std::stoll(val.data); } catch(...) { ival=0;}
    ival += op.inc;
  }
  persistent_value::serialize<slice_type>(*new_value, std::to_string(ival), field_type::number, op.ttl);
  return true;
}

bool merge_operator::upd_(const slice_type& key,
                          const slice_type* existing_slice,
                          const slice_type& value_op,
                          std::string* new_value,
                          ::rocksdb::Logger* /*logger*/) const
{
  operation_upd op;
  operation_upd::deserialize<slice_type>(value_op, op);
  
  typedef std::pair<std::string, std::string> keyval_t;
  typedef std::vector< keyval_t > persistent_object_t;
  typedef request::upd::field::params_list_t modify_list_t;
  
  typedef ::wfc::json::value<std::string> string_json;
  typedef ::wfc::json::raw_value<std::string> raw_json;
  typedef ::wfc::json::object2array< string_json, string_json > persistent_json;
  typedef request::upd_json::array_of_params_json modify_json;
  
  modify_list_t mlist;
  persistent_object_t pobj;
  
  
  if ( existing_slice )
  {
    persistent_value pval = persistent_value::deserialize<slice_type>( *existing_slice, false );
    if ( !op.force && pval.type!=field_type::object && pval.type!=field_type::any )
    {
      COMMON_LOG_WARNING("prefixdb updater 'upd' for key=" << key.ToString() << " bad type. Not changed!")
      new_value->assign( existing_slice->data(), existing_slice->size() );
      //persistent_value::serialize<slice_type>(*new_value, pval.data, pval.type, pval.ttl);
      return true;
    }
    
    try
    {
      persistent_json::serializer()(pobj, pval.data.begin(), pval.data.end() );
    }
    catch(...)
    {
      if ( !op.force )
      {
        COMMON_LOG_WARNING("prefixdb updater 'upd' for key=" << key.ToString() << " bad persistent value. Not changed!")
        new_value->assign( existing_slice->data(), existing_slice->size() );
        //persistent_value::serialize<slice_type>(*new_value, pval.data, pval.type, pval.ttl);
        return true;
      }
      pobj.clear();
    }
  }
  
  try
  {
    modify_json::serializer()(mlist, op.val.begin(), op.val.end());
  }
  catch(...)
  {
    if ( existing_slice )
    {
      COMMON_LOG_WARNING("prefixdb updater 'upd' for key=" << key.ToString() << " bad persistent value. Not changed!")
      // persistent_value::serialize<slice_type>(*new_value, pval.data, pval.type, pval.ttl);
      new_value->assign( existing_slice->data(), existing_slice->size() );
    }
    return true;
  }
  
  
  // ----------------------------------------------
  // ----------------------------------------------
  // ----------------------------------------------
  static auto less = [](const keyval_t& left, const keyval_t& right) -> bool { return left.first < right.first;};
  if (!std::is_sorted( pobj.begin(), pobj.end(), less) )
  {
    std::sort(pobj.begin(), pobj.end(), less);
    std::unique(pobj.begin(), pobj.end(), less);
  }
  
  for (auto& m : mlist)
  {
    keyval_t fi;
    fi.first = std::move(m.key);
    auto itr = std::lower_bound(pobj.begin(), pobj.end(), fi, less );
    if ( itr == pobj.end() )
    {
      itr = pobj.insert(itr, std::move(fi) );
    }
    std::string& pval = itr->second;
    
    using parser = ::wfc::json::parser;
    ::wfc::json::value<int64_t> ser;
    if ( parser::is_number( m.inc.begin(), m.inc.end() ) )
    {
      int64_t val = 0;
      if ( parser::is_number( pval.begin(), pval.end() ) )
      {
        ser(val, pval.begin(), pval.end() );
      }
      int64_t inc = 0;
      ser(inc, m.inc.begin(), m.inc.end() );
      val += inc;
      pval.clear();
      ser(val, std::inserter(pval, pval.end() ) );
    }
    else if ( !parser::is_null( m.val.begin(), m.val.end() ) )
    {
      pval = std::move(m.val);
    }
  }

  std::string pobj_j;
  pobj_j.reserve( pobj.size() * 10 );
  persistent_json::serializer()(pobj, std::inserter(pobj_j, pobj_j.end()) );
  
  persistent_value::serialize<slice_type>(*new_value, pobj_j, field_type::object, op.ttl);
  return true;
}

}}
