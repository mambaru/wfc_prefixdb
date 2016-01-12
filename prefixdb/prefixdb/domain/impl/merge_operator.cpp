
#include <rocksdb/merge_operator.h>
#include <rocksdb/env.h>
#include "value.hpp"
#include "merge_operator.hpp"

namespace wamba{ namespace prefixdb{
  
bool merge_operator::Merge(const slice_type& key,
                     const slice_type* existing_value,
                     const slice_type& value,
                     std::string* new_value,
                     ::rocksdb::Logger* logger) const 
{
  operation op = *( reinterpret_cast<const operation*>(value.data()) );
  if ( op == operation::inc)
  {
    return this->inc_(key, existing_value, value, new_value, logger);
  }
  else
  {
    ::rocksdb::Error(logger, "merge_operator error: unknown operation");
  }
  return false;
}
  
const char* merge_operator::Name() const 
{
  return "PreffixIncUpdOperator";
}
  
bool merge_operator::inc_(const slice_type& /*key*/,
                          const slice_type* existing_slice,
                          const slice_type& value_op,
                          std::string* new_value,
                          ::rocksdb::Logger* /*logger*/) const
{
  
  operation_inc op = operation_inc::deserialize<slice_type>(value_op);

  int64_t ival = op.def;
  if ( existing_slice )
  {
    value val = value::deserialize<slice_type>( *existing_slice, false );
    if ( !op.force && op.type != val.type )
    {
      return false;
    }
    ival = std::stoll(val.data);
    ival += op.inc;
  }
  value::serialize<slice_type>(*new_value, std::to_string(ival), op.type, op.ttl);
  return true;
}

}}
