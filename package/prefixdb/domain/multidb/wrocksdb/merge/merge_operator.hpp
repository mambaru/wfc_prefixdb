#pragma once

#include <rocksdb/merge_operator.h>
#include <rocksdb/env.h>

#include "merge.hpp"
#include "packed.hpp"
#include "packed_params.hpp"

#include <deque>
#include <vector>
#include <atomic>

namespace wamba{ namespace prefixdb{

class merge_operator
  : public ::rocksdb::MergeOperator
{
public:
  typedef ::rocksdb::Slice slice_type;
  typedef std::vector<std::string> update_list;

  merge_operator(const std::string& name, size_t array_limit, size_t packed_limit );

  virtual const char* Name() const override;

  virtual bool FullMergeV2(const MergeOperationInput& merge_in,
                           MergeOperationOutput* merge_out) const override;

private:

  void setnx_(const slice_type* value, const update_list& operands, std::string& result) const;
  void inc_(const slice_type* value, const update_list& operands, std::string& result) const;
  void inc_operand_(const std::string& operand, int64_t& num, bool exist) const;

  void add_(const slice_type* value, const update_list& operands, std::string& result) const;
  void add_operand_(const std::string& operand, std::deque<std::string>& arr) const;

  void packed_(const slice_type* value, const update_list& operands, std::string& result) const;
  void packed_operand_(const std::string& operand, packed_t& pck) const;
  void packed_inc_(const packed_field_params& upd, std::string& result) const;

  std::string _name;
  std::atomic<size_t> _array_limit;
  std::atomic<size_t> _packed_limit;
};

}}
