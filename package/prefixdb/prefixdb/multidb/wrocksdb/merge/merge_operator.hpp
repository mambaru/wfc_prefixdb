#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <rocksdb/merge_operator.h>
#include <rocksdb/env.h>
#pragma GCC diagnostic pop

#include "merge.hpp"
#include "packed.hpp"
#include "packed_params.hpp"

#include <deque>
#include <atomic>

namespace wamba{ namespace prefixdb{
  
class merge_operator
  : public ::rocksdb::MergeOperator
{
public:
  typedef ::rocksdb::Slice slice_type;
  typedef ::rocksdb::Logger logger_type;
  typedef std::deque<std::string> operand_list;
  typedef std::vector<std::string> update_list;
  typedef std::function< void(const std::string& key)> compact_handler;
  
  merge_operator(size_t array_limit, size_t packed_limit );
  
  virtual const char* Name() const override;
 
  virtual bool FullMergeV2(const MergeOperationInput& merge_in,
                           MergeOperationOutput* merge_out) const override; 

  
  /*virtual bool AllowSingleOperand() const override ;*/

  /*virtual bool PartialMerge(const rocksdb::Slice& key,
                                const rocksdb::Slice& left_operand,
                                const rocksdb::Slice& right_operand,
                                std::string* new_value,
                                rocksdb::Logger* logger) const override ;
                                */
  // Allows to control when to invoke a full merge during Get.
  // This could be used to limit the number of merge operands that are looked at
  // during a point lookup, thereby helping in limiting the number of levels to
  // read from.
  // Doesn't help with iterators.
  
  /*virtual bool ShouldMerge(const std::vector<slice_type>& ) const override;*/
private:

  void setnx_(const slice_type* value, const update_list& operands, std::string& result) const;
  void inc_(const slice_type* value, const update_list& operands, std::string& result) const;
  /*void partial_inc_(merge&& mrg1, merge&& mrg2, std::string& result) const;*/
  void inc_operand_(const std::string& operand, int64_t& num, bool exist) const;

  void add_(const slice_type* value, const update_list& operands, std::string& result) const;
  void add_operand_(const std::string& operand, std::deque<std::string>& arr) const;

  void packed_(const slice_type* value, const update_list& operands, std::string& result) const;
  void packed_operand_(const std::string& operand, packed_t& pck) const;
  void packed_inc_(const packed_field_params& upd, std::string& result) const;
  
  std::atomic<size_t> _array_limit;
  std::atomic<size_t> _packed_limit;
};

}}
