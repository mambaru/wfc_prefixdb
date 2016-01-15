#pragma once

#include "basic_operation.hpp"
#include <memory>
#include <iostream>

namespace wamba{ namespace prefixdb{
 
struct operation_set: basic_operation<operation::set> {};
  
//typedef basic_operation<operation::set> operation_set;

  /*
struct base_op_set: operation_base<operation::set> 
{
  size_t data_size = 0;
};


struct operation_set: base_op_set
{

  std::string val;

  template<typename SliceType>
  inline static SliceType serialize(buffer_type& buff, const operation_set& op)
  {
    size_t old_size = buff.size();
    size_t op_size = sizeof(base_op_set);
    
    const char* beg = reinterpret_cast<const char*>(&op);
    const char* end = beg + op_size;
    std::copy( beg, end,  std::inserter(buff, buff.end()) );
    std::copy( op.val.begin(), op.val.end(), std::inserter(buff, buff.end()) );
    return SliceType(buff.data() + old_size, op_size + op.val.size());
  }
  
  template<typename SliceType>
  inline static operation_set deserialize( const SliceType& slice )
  {
    operation_set op;
    static_cast<base_op_set&>(op) = base_op_set(*reinterpret_cast<const operation_set*>(slice.data()));
    op.val.assign(slice.data() + sizeof(base_op_set), slice.data() + slice.size());
    return op;
    //return *reinterpret_cast<const operation_inc*>(slice.data());
  }
};
*/

}}