#include "wal_buffer.hpp"
#include <iow/workflow/rrqueue.hpp>
#include <prefixdb/logger.hpp>

namespace wamba{ namespace prefixdb {

struct log_record
{
  typedef std::vector<char> data_type;
  data_type log;
  template<typename Beg, typename End>
  log_record(Beg beg, End end): log(beg, end){}
  
  uint64_t get_sequence_number() const
  {
    return log_record::get_sequence_number(log);
  }
  
  static uint64_t get_sequence_number(const data_type& log)
  {
    return *reinterpret_cast<const uint64_t*>( &(log[0]) );
  }
};
  
class wal_buffer::impl 
  : public ::iow::circular_buffer<log_record>
{
public:
  impl(size_t size): circular_buffer<log_record>(size){};
};

wal_buffer::wal_buffer( const std::string& name, size_t size)
  : _name(name)
{
  _buffer = std::make_shared<impl>(size);
}

uint64_t wal_buffer::get_sequence_number(const data_type& log)
{
  return log_record::get_sequence_number(log);
}


void wal_buffer::add(const std::string& log)
{
  _buffer->push_back( log_record(log.begin(), log.end()));
}

bool wal_buffer::empty() const
{
  return _buffer->empty();
}

uint64_t wal_buffer::get_first_number() const
{
  if ( this->empty() ) return -1;
  return _buffer->front().get_sequence_number();
}

uint64_t wal_buffer::get_last_number() const
{
  if ( this->empty() ) return -1;
  return _buffer->back().get_sequence_number();
  
}

bool wal_buffer::may_exist(uint64_t sequence_number) const
{
  if (this->empty()) return false;
  return sequence_number >= this->get_first_number() && sequence_number <=this->get_last_number();
}

bool wal_buffer::get_updates_since(uint64_t sequence_number, size_t limit, log_list& res )
{
  if ( !this->may_exist(sequence_number) )
  {
    PREFIXDB_LOG_DEBUG("wal_buffer::get_updates_since not ready for " << sequence_number << " [" << this->get_first_number() << "," << this->get_last_number() << "]" )
    return false;
  }
  
  const char *pnum = reinterpret_cast<const char*>(&sequence_number);
  log_record rec(pnum, pnum + sizeof(sequence_number) );
  res.reserve(limit);
  auto itr = std::lower_bound( _buffer->begin(), _buffer->end(), rec, [](const log_record& left, const log_record& right) ->bool
  {
      return  left.get_sequence_number() < right.get_sequence_number();
  });
  
  if ( itr == _buffer->end() )
    itr = (_buffer->rbegin() + 1).base();
  
  if ( sequence_number > itr->get_sequence_number() )
  {
    if ( itr != _buffer->begin() )
      --itr;
  }
  
  if ( sequence_number > itr->get_sequence_number() )
    return false;
 
  res.reserve(limit);
  for (size_t i = 0; i!=limit && itr!=_buffer->end(); ++i, ++itr)
  {
    res.push_back( itr->log );
  }
  
  PREFIXDB_LOG_DEBUG("wal_buffer::get_updates_since " << res.size() );
  return !res.empty();
}


}}