
#include "since_reader.hpp"
#include <prefixdb/logger.hpp>
#include <wfc/memory.hpp>
#include <wfc/wfc_exit.hpp>
//#include <db/log_format.h>
#include <wjson/wjson.hpp>

namespace wamba{ namespace prefixdb {

namespace
{
  inline size_t get_size(const char** pbeg, const char* end)
  {
    if ( *pbeg == end ) return 0;

    size_t size = 0;
    bool fin = false;
    for (int i=0; !fin && i<8; i++)
    {
      size_t cnk = static_cast<size_t>( *reinterpret_cast<const uint8_t*>(*pbeg) );
      fin = ((cnk >> 7) & 1) == 0;
      cnk &= ~( static_cast<size_t>(1) << 7);
      cnk <<= 7*i;
      size |= cnk;
      (*pbeg)++;
      if ( *pbeg == end && !fin)
      {
        PREFIXDB_LOG_FATAL("prefixdb::since_reader::get_size log format error");
        return 0;
      }
    }
    return size;
  }

  inline ::rocksdb::Slice get_item(const char** beg, const char* end, bool json_flag)
  {
    size_t size = get_size(beg, end);
    const char* last = *beg + size;
    if ( json_flag )
      last-=4;
    if ( last > end || last < *beg)
    {
      PREFIXDB_LOG_FATAL("prefixdb::since_reader::get_item size error");
      return ::rocksdb::Slice();
    }
    auto jend = last;
    if ( json_flag )
    {
      // Парсим, чтобы отрезать мусор
      wjson::json_error er;
      jend = wjson::parser::parse_value(*beg, last, &er);
      if ( er )
      {
        // Костыль для варианта 4.мусор (предполагаем, что целое число)
        if ( wjson::parser::is_number(*beg, last) )
        {
          er.reset();
          jend = wjson::parser::parse_integer(*beg, last, &er);
        }

        if ( er )
        {
          PREFIXDB_LOG_FATAL("prefixdb::since_reader::get_item json error. " << wjson::strerror::message_trace(er, *beg, last) );
          return ::rocksdb::Slice();
        }
      }

      if ( jend!=last )
      {
        PREFIXDB_LOG_WARNING("prefixdb::since_reader::get_item фиксация ошибки протокола. '" << std::string(*beg, last)
                             << "'->'" << std::string(*beg, jend) << "'" );
      }
    }
    size_t jsize = static_cast<size_t>(std::distance(*beg, jend));
    if ( jsize <= size )
    {
      ::rocksdb::Slice res(*beg, jsize);
      *beg += size;
      return res;
    }
    PREFIXDB_LOG_FATAL("prefixdb::since_reader::get_item distance error");
    return ::rocksdb::Slice();
  }

  inline ::rocksdb::Slice get_key(const char** beg, const char* end)
  {
    ::rocksdb::Slice res = get_item(beg, end, false);
    if ( 0 == res.compare("~slave-last-sequence-number~") )
      return ::rocksdb::Slice();
    return res;
  }

  inline ::rocksdb::Slice get_val(const char** beg, const char* end)
  {
    return get_item(beg, end, true);
  }

  inline std::pair< ::rocksdb::Slice, ::rocksdb::Slice> get_pair(const char** beg, const char* end)
  {
    auto first  = get_key(beg, end);
    auto second = get_val(beg, end);
    return std::make_pair(first, second);
  }
}

namespace{
inline std::string s4l( ::rocksdb::Slice s)
{
  // Slice for log
  if ( s.size() < 84 )
    return s.ToString();

  std::string res;
  res.append(s.data(), s.data() + 40);
  res += "...";
  res.append(s.data() + s.size() - 40, s.data() + s.size());
  return res;
}
}


since_reader::since_reader(const std::string& name)
  : _name(name)
{

}

const char*  since_reader::read_put_(const char* beg, const char* end, bool ignore)
{
  auto pair = get_pair( &beg, end);

  if ( !pair.first.empty() )
  {
    if ( !ignore )
    {
      this->_batch->Put( pair.first, pair.second );
      PREFIXDB_LOG_REPLI(_log, "Put " << s4l(pair.first) << "=" << s4l(pair.second) )
    }
  }
  else
  {
    PREFIXDB_LOG_ERROR("Replication Put");
  }
  return beg;
}

const char*  since_reader::read_del_(const char* beg, const char* end, bool ignore)
{
  auto key = get_key( &beg, end);
  if ( !key.empty() )
  {
    if ( !ignore )
    {
      this->_batch->Delete( key );
      PREFIXDB_LOG_REPLI(_log, "Delete " << s4l(key) )
    }
  }
  else
  {
    PREFIXDB_LOG_ERROR("Replication Delete empty key");
  }
  return beg;
}

const char*  since_reader::read_merge_(const char* beg, const char* end, bool ignore)
{
  auto pair = get_pair( &beg, end);
  if ( !pair.first.empty() )
  {
    if ( !ignore )
    {
      this->_batch->Merge( pair.first, pair.second );
      PREFIXDB_LOG_REPLI(_log, "Merge " << s4l(pair.first) << "=" << s4l(pair.second) )
    }
  }
  else
  {
    PREFIXDB_LOG_ERROR("Replication Merge");
  }
  return beg;
}

const char*  since_reader::read_op_(const char* beg, const char* end, bool ignore)
{
  if (beg > end)
  {
    PREFIXDB_LOG_FATAL("Iterators error");
    abort();
    return nullptr;
  }
  if (beg==end) return nullptr;
  int type = int( *reinterpret_cast<const uint8_t*>(beg++) );
  switch ( type )
  {
    case 0:
      beg = read_del_(beg, end, ignore);
      break;
    case 1:
      beg = read_put_(beg, end, ignore);
      break;
    case 2:
      beg = read_merge_(beg, end, ignore);
      break;
    default:
      PREFIXDB_LOG_FATAL("prefixdb::since_reader::read_item_ unknown operation");
      return nullptr;
  };
  ++_op_counter;
  return beg;
}

unsigned int since_reader::read_record_(const char *beg, const char *end)
{
  uint64_t sn = *reinterpret_cast<const uint64_t *>( static_cast<const void*>(beg) );
  const unsigned int type = static_cast<unsigned int>(beg[8]);
  size_t head = 12;
  beg += head;
  while ( nullptr != (beg = this->read_op_(beg, end, sn < _next_seq_number)) ) ++sn;
  _next_seq_number = sn;
  return type;
}

size_t since_reader::parse_()
{
  if ( _batch == nullptr )
    _batch = std::make_unique<batch_type>();
  const char *beg = &(_buffer[0]);
  const char *end = beg + _buffer.size();

  this->read_record_(beg, end);
  return static_cast<size_t>( std::distance(beg, end) );
}

void since_reader::reset()
{
  _status = status::Ready;
  _buffer.clear();
}

bool since_reader::parse(const data_type& data)
{
  if ( _status == status::Error ) return false;
  _buffer.reserve(data.size());
  std::copy( data.begin(), data.end(), std::inserter(_buffer, _buffer.end()) );
  if ( size_t s = this->parse_() )
  {
    _buffer.erase( _buffer.begin(), _buffer.begin() + static_cast<ptrdiff_t>(s) );
  }
  return _status != status::Error;
}

since_reader::batch_ptr since_reader::detach()
{
  _op_counter = 0;
  if ( _batch == nullptr )
    return std::make_unique<batch_type>();
  return std::move(_batch);
}

size_t since_reader::op_count() const
{
  return _op_counter;
}

const since_reader::data_type& since_reader::buffer() const
{
  return _buffer;
}

size_t since_reader::size() const
{
  return _buffer.size();
}


bool since_reader::empty() const
{
  return _buffer.empty();
}

}}
