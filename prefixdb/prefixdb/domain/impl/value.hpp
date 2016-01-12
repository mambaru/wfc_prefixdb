#pragma once

#include <prefixdb/api/aux/field_type.hpp>
#include <memory>
#include <iostream>

namespace wamba{ namespace prefixdb{
 
struct value_head
{
  const size_t size = sizeof(value_head);
  time_t       ttl  = 0;
  field_type   type = field_type::any;
  
  /*
  value_head(field_type type, time_t ttl)
    : type(type), ttl(ttl) {}
    */
};

struct value : value_head
{
  typedef std::string data_type;
  typedef std::vector<char> buffer_type;
  data_type  data;
  
  /*
  value(data_type data, field_type type, time_t ttl)
    : value_head(type, ttl)
    , data(std::move(data) )
  {}
  */
  
  template<typename SliceType, typename BufferType>
  inline static SliceType serialize(BufferType& buff, const data_type& val, field_type type, time_t ttl)
  {
    std::cout << "serialize val=" << val << std::endl;
    size_t old_size = buff.size();
    size_t value_size = sizeof(value_head) + val.size();
    buff.reserve(buff.size() + value_size);
    value_head head;
    head.type = type;
    head.ttl = ttl;
    
    const char* beg = reinterpret_cast<const char*>(&head);
    const char* end = beg + sizeof(head);
    //auto inserter = std::inserter(buff, buff.end());
    std::copy( beg, end,  std::inserter(buff, buff.end()));
    std::copy( val.begin(), val.end(), std::inserter(buff, buff.end()));
    std::cout << "ser size: " << buff.size() << " val size:" << val.size() << " head size: " << sizeof(value_head) << std::endl;
    std::cout << "ser head_size: " << head.size << std::endl;
    std::cout << "ser [" << buff.data() + old_size + 24 << "]"<< std::endl;
    return SliceType(buff.data() + old_size, value_size);
  }
  
  inline static value deserialize(const char* beg, const char* end, bool noval)
  {
    value result = value();
    size_t size = *reinterpret_cast<const size_t*>( beg );
    const char* cur = beg + sizeof(size);
    if ( sizeof(time_t) < (size_t)std::distance(cur, end) )
    {
      result.ttl = *reinterpret_cast<const time_t*>( cur );
      cur += sizeof(time_t);
      if ( sizeof(field_type) < (size_t)std::distance(cur, end) )
      {
	result.type = *reinterpret_cast<const field_type*>( cur );
	cur += sizeof(field_type);
      }
    }
    else
    {
      std::cout << sizeof(field_type) << " > " <<  (size_t)std::distance(cur, end) << std::endl;
    }
 
    if (!noval) result.data.assign(beg + size, end );
    
    return std::move(result);
  }
  
  template<typename SliceType>
  inline static value deserialize(const SliceType& slice, bool noval)
  {
    return std::move(value::deserialize(slice.data(), slice.data() + slice.size(), noval));
  }

};

enum class operation
{
  inc,
  upd
};

struct operation_inc
{
  const operation op = operation::inc;
  field_type type = field_type::any; // number, string, package, any, none - запрещено
  time_t ttl = 0;
  bool force = true;
  int64_t inc = 0;
  int64_t def = 0;

  typedef std::vector<char> buffer_type;
  
  /*
  operation_inc(bool force, int64_t inc, int64_t def, field_type type, time_t ttl)
    : type(type)
    , ttl(ttl)
    , force(force)
    , inc(inc)
    , def(def)
  {
  }
  */

  template<typename SliceType>
  inline static SliceType serialize(buffer_type& buff, const operation_inc& val)
  {
    size_t old_size = buff.size();
    size_t value_size = sizeof(operation_inc);
    
    const char* beg = reinterpret_cast<const char*>(&val);
    const char* end = beg + value_size;
    auto inserter = std::inserter(buff, buff.end());
    std::copy( beg, end,  inserter);
    
    return SliceType(buff.data() + old_size, value_size);
  }
  
  template<typename SliceType>
  inline static operation_inc deserialize( const SliceType& slice )
  {
    return *reinterpret_cast<const operation_inc*>(slice.data());
  }

};

}}