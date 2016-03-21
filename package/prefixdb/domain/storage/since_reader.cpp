
#include "since_reader.hpp"

namespace wamba{ namespace prefixdb {
  
namespace{
  inline since_reader::status parse_buffer(since_reader::status statys, since_reader::data_type&, since_reader::item_list&)
  {
    
  }
}

void since_reader::reset()
{
  _status == status::Ready;
  _buffer.clear();
}

bool since_reader::parse(const data_type& data)
{
  if ( _status == status::Error ) return false;
  _buffer.reserve(data.size());
  std::copy( data.begin(), data.end(), std::inserter(_buffer, _buffer.end()) );
  _status = parse_buffer(_status, _buffer, _item_list);
  return _status != status::Error;
}

since_reader::item_info since_reader::pop()
{
  item_info itm = std::move(_item_list.front());
  _item_list.pop_front();
  return std::move(itm);
}

const since_reader::data_type& since_reader::buffer() const
{
  return _buffer;
}

size_t since_reader::size() const
{
  size_t res = 0;
  for (const auto& v : _item_list )
  {
    res += sizeof( v );
    res += v.key.size();
    res += v.value.size();
  }
  res += _buffer.size();
  return res;
}

size_t since_reader::count() const
{
  return _item_list.size();
}

bool since_reader::empty() const
{
  return _item_list.empty();
}

bool since_reader::buffer_empty() const
{
  return _buffer.empty();
}

size_t since_reader::buffer_size() const
{
  return _buffer.size();
}


}}
