#pragma once

#include <rocksdb/merge_operator.h>
#include <rocksdb/env.h>
#include "merge_config.hpp"
#include "packed.hpp"
#include "packed_params.hpp"

#include <deque>

namespace wamba{ namespace prefixdb{
  
class merge_operator
  : public ::rocksdb::MergeOperator
  //: public ::rocksdb::AssociativeMergeOperator
{
  
public:
  typedef ::rocksdb::Slice slice_type;
  typedef ::rocksdb::Logger logger_type;
  typedef std::deque<std::string> operand_list;
  typedef std::vector<std::string> update_list;
  
  merge_operator();
  void reconfigure(const merge_config& config);
  
  /*
  virtual bool Merge(const slice_type& key,
                     const slice_type* existing_value,
                     const slice_type& value,
                     std::string* new_value,
                     ::rocksdb::Logger* logger) const override;
                     */
 
  virtual const char* Name() const override;
 
  /*
  virtual bool PartialMerge(
    const slice_type& key,
    const slice_type& left_operand,
    const slice_type& right_operand,
    std::string* new_value,
    logger_type* logger) const override {
      return false;
    }
    */
    
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

  void inc_(const slice_type* value, const update_list& operands, std::string& result) const;
  void inc_operand_(const std::string& operand, int64_t& num, bool exist) const;

  void add_(const slice_type* value, const update_list& operands, std::string& result) const;
  void add_operand_(const std::string& operand, std::deque<std::string>& arr) const;

  void packed_(const slice_type* value, const update_list& operands, std::string& result) const;
  void packed_operand_(const std::string& operand, packed_t& pck) const;
  void packed_inc_(const packed_field_params& upd, std::string& result) const;
  //void packed_field_(const packed_field_params& upd, packed_field& field );
  std::shared_ptr<merge_config> _config;
};

}}
