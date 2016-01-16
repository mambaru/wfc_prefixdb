#pragma once

#include <rocksdb/merge_operator.h>
#include <rocksdb/env.h>

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

  void inc_(std::string& out, std::string&& upd, const char* beg, const char* end ) const;
  void packed_(std::string& out, std::string&& upd, const char* beg, const char* end ) const;
  
};

}}
