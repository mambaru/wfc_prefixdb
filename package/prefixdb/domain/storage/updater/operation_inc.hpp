#pragma once

#include "operation_base.hpp"
#include "basic_operation.hpp"
#include <memory>
#include <iostream>

namespace wamba{ namespace prefixdb{
 
template<operation op>
struct base_inc: operation_base<op>
{
  int64_t inc = 0;
  int64_t def = 0;
};

struct operation_inc: basic_operation<operation::inc, base_inc> {};
 
/*
struct operation_inc: operation_base<operation::inc>
{
  int64_t inc = 0;
  int64_t def = 0;
  
  typedef std::vector<char> buffer_type;
  
  template<typename SliceType>
  inline static SliceType serialize(buffer_type& buff, const operation_inc& val)
  {
    size_t old_size = buff.size();
    size_t value_size = sizeof(operation_inc);
    
    const char* beg = reinterpret_cast<const char*>(&val);
    const char* end = beg + value_size;
    std::copy( beg, end,  std::inserter(buff, buff.end()) );
    
    return SliceType(buff.data() + old_size, value_size);
  }
  
  template<typename SliceType>
  inline static void deserialize( const SliceType& slice, operation_inc& val )
  {
    val = *reinterpret_cast<const operation_inc*>(slice.data());
  }
};
*/

}}