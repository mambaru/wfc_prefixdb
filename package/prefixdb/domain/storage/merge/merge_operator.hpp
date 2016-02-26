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
  typedef ::rocksdb::Logger logger_type;
  typedef std::deque<std::string> operand_list;
  typedef std::vector<std::string> update_list;
  
  merge_operator();
  void reconfigure(const merge_config& config);
  
  virtual bool Merge(const slice_type& key,
                     const slice_type* existing_value,
                     const slice_type& value,
                     std::string* new_value,
                     ::rocksdb::Logger* logger) const override;
 
  virtual const char* Name() const;
 
  
  virtual bool PartialMerge(
    const slice_type& key,
    const slice_type& left_operand,
    const slice_type& right_operand,
    std::string* new_value,
    logger_type* logger) const override {
      return false;
    }
    
  virtual bool FullMerge(
    const slice_type& key,
    const slice_type* value,
    const operand_list& operands,
    std::string* result,
    logger_type* logger) const override;
private:

  void inc_(std::string& out, std::string&& upd, const char* beg, const char* end ) const;
  void add_(std::string& out, std::string&& upd, const char* beg, const char* end ) const;
  void packed_(std::string& out, std::string&& upd, const char* beg, const char* end ) const;
  
  void packed_(const slice_type* value, const update_list& operands, std::string& result) const;
  std::shared_ptr<merge_config> _config;
};

}}
