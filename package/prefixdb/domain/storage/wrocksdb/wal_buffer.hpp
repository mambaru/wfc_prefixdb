#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>

namespace wamba{ namespace prefixdb {

class wal_buffer
{
  class impl;
  typedef std::mutex mutex_type;
public:
  typedef std::vector<char> data_type;
  typedef std::vector<data_type> log_list;
  wal_buffer( const std::string& name, size_t size);
  void add(const std::string& log);
  bool empty() const;
  uint64_t get_first_number() const;
  uint64_t get_last_number() const;
  bool may_exist(uint64_t sequence_number) const;
  bool get_updates_since(uint64_t sequence_number, size_t limit, log_list& res );
  
  static uint64_t get_sequence_number(const data_type& log);
private:
  bool empty_() const;
  uint64_t get_first_number_() const;
  uint64_t get_last_number_() const;
  bool may_exist_(uint64_t sequence_number) const;
  bool get_updates_since_(uint64_t sequence_number, size_t limit, log_list& res );
private:
  std::string _name;
  mutable mutex_type _mutex;
  bool _need_sort;
  std::shared_ptr<impl> _buffer;
};
  
}}