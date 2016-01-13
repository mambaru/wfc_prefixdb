
#include <rocksdb/merge_operator.h>
#include <rocksdb/env.h>
#include "../persistent_value.hpp"
#include "merge_operator.hpp"
#include "operation_inc.hpp"
#include "operation_set.hpp"

#include <wfc/logger.hpp>


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
  operation_set op = operation_set::deserialize<slice_type>(value_op);
  
  if ( existing_slice )
  {
    persistent_value val = persistent_value::deserialize<slice_type>( *existing_slice, false );
    if ( !op.force && val.type!=op.type && val.type!=field_type::any )
    {
      COMMON_LOG_WARNING("prefixdb updater set for key=" << key.ToString() << " bad type. Not changed!")
      persistent_value::serialize<slice_type>(*new_value, val.data, val.type, val.ttl);
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
  operation_inc op = operation_inc::deserialize<slice_type>(value_op);
  int64_t ival = op.def;
  if ( existing_slice )
  {
    persistent_value val = persistent_value::deserialize<slice_type>( *existing_slice, false );
    if ( !op.force && val.type!=field_type::number )
    {
      COMMON_LOG_WARNING("prefixdb updater inc for key=" << key.ToString() << " bad type. Not changed!")
      return false;
    }
    try{ ival = std::stoll(val.data); } catch(...) { ival=0;}
    ival += op.inc;
  }
  persistent_value::serialize<slice_type>(*new_value, std::to_string(ival), field_type::number, op.ttl);
  return true;
}


}}
