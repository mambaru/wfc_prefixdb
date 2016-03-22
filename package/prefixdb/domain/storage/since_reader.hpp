#pragma once

#include <rocksdb/write_batch.h>
#include <deque>
#include <vector>
#include <memory>
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
  typedef ::rocksdb::WriteBatch batch_type;
  typedef std::unique_ptr<batch_type> batch_ptr;
  
  // сборс состояний после ошибки
  void reset();
  // @return false if has errors. see message() for details
  bool parse(const data_type& data);
  item_info pop();
  batch_ptr pop_batch();
  const data_type& buffer() const;
  size_t size() const;
  size_t count() const;
  bool empty() const;
  bool buffer_empty() const;
  size_t buffer_size() const;
  
private:
  size_t parse_();
  unsigned int read_record_(const char* beg, const char* end);
  const char*  read_op_(const char* beg, const char* end);
  const char*  read_put_(const char* beg, const char* end);
  const char*  read_del_(const char* beg, const char* end);
  const char*  read_merge_(const char* beg, const char* end);
private:
  status _status = status::Ready;
  item_list _item_list;
  data_type _buffer;
  batch_ptr _batch;
};
 
}}
