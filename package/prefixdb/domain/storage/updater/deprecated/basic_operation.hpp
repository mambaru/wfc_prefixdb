#pragma once

#include "operation_base.hpp"
#include <memory>
#include <iostream>

namespace wamba{ namespace prefixdb{

  /*
template<operation Op>
struct basic_op_head: operation_base<Op> 
{
  size_t data_size = 0;
};
*/

template<operation Op, template<operation> class base = operation_base >
struct basic_operation: base<Op>
{
  typedef basic_operation<Op, base> self;
  typedef base<Op> head;
  typedef typename head::buffer_type buffer_type;
  std::string val;

  template<typename SliceType>
  inline static SliceType serialize(buffer_type& buff, const self& op)
  {
    size_t old_size = buff.size();
    size_t op_size = sizeof(head);
    
    const char* beg = reinterpret_cast<const char*>(&op);
    const char* end = beg + op_size;
    std::copy( beg, end,  std::inserter(buff, buff.end()) );
    std::copy( op.val.begin(), op.val.end(), std::inserter(buff, buff.end()) );
    return SliceType(buff.data() + old_size, op_size + op.val.size());
  }
  
  template<typename SliceType>
  inline static void deserialize( const SliceType& slice, self& val )
  {
    self op;
    static_cast<head&>(op) = head(*reinterpret_cast<const self*>(slice.data()));
    op.val.assign(slice.data() + sizeof(head), slice.data() + slice.size());
  }
};

}}