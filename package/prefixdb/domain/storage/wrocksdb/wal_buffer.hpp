#pragma once

#include <string>
#include <vector>
#include <memory>

namespace wamba{ namespace prefixdb {

class wal_buffer
{
  class impl;
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
  std::string _name;
  std::shared_ptr<impl> _buffer;
};
  
}}