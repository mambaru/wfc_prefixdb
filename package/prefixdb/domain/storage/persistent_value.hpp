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
};

struct persistent_value : value_head
{
  typedef std::string data_type;
  typedef std::vector<char> buffer_type;
  data_type  data;
  
  template<typename SliceType, typename BufferType>
  inline static SliceType serialize(BufferType& buff, const data_type& val, field_type type, time_t ttl)
  {
    //std::vector<char> buff;
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
    return SliceType(buff.data() + old_size, value_size);
  }
  
  inline static persistent_value deserialize(const char* beg, const char* end, bool noval)
  {
    persistent_value result = persistent_value();
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
  inline static persistent_value deserialize(const SliceType& slice, bool noval)
  {
    return std::move(persistent_value::deserialize(slice.data(), slice.data() + slice.size(), noval));
  }

};

}}