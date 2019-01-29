#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsign-conversion"

#include <rocksdb/write_batch.h>
#pragma GCC diagnostic pop

#include <deque>
#include <vector>
#include <memory>
#include <cstddef>

namespace wamba{ namespace prefixdb{
  
class since_reader
{
public:
  typedef std::vector<char> data_type;
  enum class status
  {
    Error = 0,
    Ready = 1
  };
  
  typedef ::rocksdb::WriteBatch batch_type;
  typedef std::unique_ptr<batch_type> batch_ptr;
  
  void enable_log() { _log = true;}
  void disable_log() { _log = false;}
  // сборс состояний после ошибки
  void reset();
  // @return false if has errors. see message() for details
  bool parse(const data_type& data);
  batch_ptr detach();
  const data_type& buffer() const;
  size_t size() const;
  bool empty() const;
  uint64_t get_next_seq_number() const { return _next_seq_number;}
private:
  size_t parse_();
  unsigned int read_record_(const char* beg, const char* end);
  const char*  read_op_(const char* beg, const char* end, bool ignore);
  const char*  read_put_(const char* beg, const char* end, bool ignore);
  const char*  read_del_(const char* beg, const char* end, bool ignore);
  const char*  read_merge_(const char* beg, const char* end, bool ignore);
private:
  status _status = status::Ready;
  uint64_t _next_seq_number = 0;
  data_type _buffer;
  batch_ptr _batch;
  bool _log = false; // TODO: выключить
};
 
}}
