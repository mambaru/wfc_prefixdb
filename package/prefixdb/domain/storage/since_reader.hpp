#pragma once

#include <vector>
#include <deque>
#include <cstddef>

namespace wamba{ namespace prefixdb{
  
class since_reader
{
public:
  typedef std::vector<char> data_type;
  enum class item_type
  {
    None  = -1,
    Del   = 0,
    Put   = 1,
    Merge = 2
  };
  
  enum class status
  {
    Error = 0,
    Ready = 1
  };
  
  struct item_info
  {
    item_type type = item_type::None;
    data_type key;
    data_type value;
  };
  typedef std::deque<item_info> item_list;
  
  // сборс состояний после ошибки
  void reset();
  // @return false if has errors. see message() for details
  bool parse(const data_type& data);
  item_info pop();
  const data_type& buffer() const;
  size_t size() const;
  size_t count() const;
  bool empty() const;
  bool buffer_empty() const;
  size_t buffer_size() const;

private:
  
  status _status = status::Ready;
  item_list _item_list;
  data_type _buffer;
};
 
}}
