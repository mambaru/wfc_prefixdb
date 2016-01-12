#pragma once

#include <rocksdb/merge_operator.h>
#include <rocksdb/env.h>
#include "value.hpp"

namespace wamba{ namespace prefixdb{
  
class merge_operator
  : public ::rocksdb::AssociativeMergeOperator
{
  
public:
  typedef ::rocksdb::Slice slice_type;
  
  virtual bool Merge(const slice_type& key,
                     const slice_type* existing_value,
                     const slice_type& value,
                     std::string* new_value,
                     ::rocksdb::Logger* logger) const override;
 
  virtual const char* Name() const;
  
private:

  bool inc_(const slice_type& key,
            const slice_type* existing_value,
            const slice_type& value,
            std::string* new_value,
            ::rocksdb::Logger* logger) const;

};

}}
