#pragma once

#include <rocksdb/merge_operator.h>
#include <rocksdb/env.h>
#include "merge_config.hpp"

namespace wamba{ namespace prefixdb{
  
class merge_operator
  : public ::rocksdb::AssociativeMergeOperator
{
  
public:
  typedef ::rocksdb::Slice slice_type;
  
  merge_operator();
  void reconfigure(const merge_config& config);
  
  virtual bool Merge(const slice_type& key,
                     const slice_type* existing_value,
                     const slice_type& value,
                     std::string* new_value,
                     ::rocksdb::Logger* logger) const override;
 
  virtual const char* Name() const;
  
private:

  void inc_(std::string& out, std::string&& upd, const char* beg, const char* end ) const;
  void add_(std::string& out, std::string&& upd, const char* beg, const char* end ) const;
  void packed_(std::string& out, std::string&& upd, const char* beg, const char* end ) const;
  std::shared_ptr<merge_config> _config;
};

}}
