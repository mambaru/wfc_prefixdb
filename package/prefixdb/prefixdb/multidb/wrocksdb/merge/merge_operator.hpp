#pragma once

#include <rocksdb/merge_operator.h>
#include <rocksdb/env.h>
#include "packed.hpp"
#include "packed_params.hpp"

#include <deque>
#include <atomic>

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
  typedef std::function< void(const std::string& key)> compact_handler;
  
  merge_operator(size_t array_limit, size_t packed_limit );
  
  void set_handler( compact_handler );
  
  //void reconfigure(const merge_config& config);
  virtual const char* Name() const override;
 
  virtual bool FullMerge(
    const slice_type& key,
    const slice_type* value,
    const operand_list& operands,
    std::string* result,
    logger_type* logger) const override;
private:

  void setnx_(const slice_type* value, const update_list& operands, std::string& result) const;
  void inc_(const slice_type* value, const update_list& operands, std::string& result) const;
  void inc_operand_(const std::string& operand, int64_t& num, bool exist) const;

  void add_(const slice_type* value, const update_list& operands, std::string& result) const;
  void add_operand_(const std::string& operand, std::deque<std::string>& arr) const;

  void packed_(const slice_type* value, const update_list& operands, std::string& result) const;
  void packed_operand_(const std::string& operand, packed_t& pck) const;
  void packed_inc_(const packed_field_params& upd, std::string& result) const;
  //void packed_field_(const packed_field_params& upd, packed_field& field );
  //std::shared_ptr<merge_config> _config;
  std::atomic<size_t> _array_limit;
  std::atomic<size_t> _packed_limit;
  compact_handler _handler;
};

}}
