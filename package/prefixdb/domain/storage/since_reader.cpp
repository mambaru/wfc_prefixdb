
#include "since_reader.hpp"
#include "db/log_format.h"
#include <iostream>
#include <prefixdb/logger.hpp>
#include <wfc/memory.hpp>
#include <wfc/core/abort.hpp>

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
      cnk &= ~(1 << 7);
      cnk <<= 7*i;
      size |= cnk;
      (*pbeg)++;
      if ( *pbeg == end && !fin) 
        ::wfc_abort("prefixdb::since_reader::get_size log format error"); 
    }
    return size;
  }
  
  inline ::rocksdb::Slice get_item(const char** beg, const char* end)
  {
    size_t size = get_size(beg, end);
    if ( *beg + size > end )
      ::wfc_abort("prefixdb::since_reader::get_item size error"); 
    ::rocksdb::Slice res(*beg, size);
    *beg += size;
    return res;
  }

  inline ::rocksdb::Slice get_key(const char** beg, const char* end)
  {
    ::rocksdb::Slice res = get_item(beg, end);
    if ( 0 == res.compare("~slave-last-sequence-number~") )
      return ::rocksdb::Slice();
    return res;
  }

  inline ::rocksdb::Slice get_val(const char** beg, const char* end)
  {
    return get_item(beg, end);
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
  
const char*  since_reader::read_put_(const char* beg, const char* end)
{
  auto pair = get_pair( &beg, end);
  
  if ( !pair.first.empty() )
  {
    this->_batch->Put( pair.first, pair.second );
    PREFIXDB_LOG_REPLI(_log, "Put " << s4l(pair.first) << "=" << s4l(pair.second) )
  }
  else
  {
    PREFIXDB_LOG_ERROR("Replication Put");
  }
  return beg;
}

const char*  since_reader::read_del_(const char* beg, const char* end)
{
  auto key = get_key( &beg, end);
  if ( !key.empty() )
  {
    this->_batch->Delete( key );
    PREFIXDB_LOG_REPLI(_log, "Delete " << s4l(key) )
  }
  else
  {
    PREFIXDB_LOG_ERROR("Replication Delete");
  }
  return beg;
}

const char*  since_reader::read_merge_(const char* beg, const char* end)
{
  auto pair = get_pair( &beg, end);
  if ( !pair.first.empty() )
  {
    this->_batch->Merge( pair.first, pair.second );
    PREFIXDB_LOG_REPLI(_log, "Merge " << s4l(pair.first) << "=" << s4l(pair.second) )
  }
  else
  {
    PREFIXDB_LOG_ERROR("Replication Merge");
  }
  return beg;
}

const char*  since_reader::read_op_(const char* beg, const char* end)
{
  if (beg > end) 
    abort();
  if (beg==end) return nullptr;
  int type = int( *reinterpret_cast<const uint8_t*>(beg++) );
  switch ( type )
  {
    case 0: 
      beg = read_del_(beg, end); 
      break;
    case 1: 
      beg = read_put_(beg, end); 
      break;
    case 2: 
      beg = read_merge_(beg, end); 
      break;
    default:
      beg = end;
      ::wfc_abort("prefixdb::since_reader::read_item_ unknown operation"); 
      abort();
      return nullptr;
  };
  
  if ( beg==end )
  {
    return nullptr;
  }
  return beg!=end ? beg : nullptr;
}

unsigned int since_reader::read_record_(const char *beg, const char *end)
{
  _seq_number = *reinterpret_cast<const uint32_t *>(beg);
  const unsigned int type = beg[8];
  size_t head = 12;
  beg += head;
  while ( nullptr != (beg = this->read_op_(beg, end)) );
  return type;
}

size_t since_reader::parse_()
{
  if ( _batch == nullptr )
    _batch = std::make_unique<batch_type>();
  const char *beg = &(_buffer[0]);
  const char *end = beg + _buffer.size();
  
  /*const unsigned int record_type =*/ 
  this->read_record_(beg, end);
  /*
  switch ( record_type )
  {
    case ::rocksdb::log::kZeroType:
      std::cout << "kZeroType" << std::endl;
      break;
    case ::rocksdb::log::kFullType:
      std::cout << "kFullType" << std::endl;
      break;
    case ::rocksdb::log::kFirstType:
      std::cout << "kFirstType" << std::endl;
      break;
    case ::rocksdb::log::kMiddleType:
      std::cout << "kMiddleType" << std::endl;
      break;
    case ::rocksdb::log::kLastType:
      std::cout << "kLastType" << std::endl;
      break;
    case ::rocksdb::log::kRecyclableFullType:
      std::cout << "kRecyclableFullType" << std::endl;
      break;
    case ::rocksdb::log::kRecyclableFirstType:
      std::cout << "kRecyclableFirstType" << std::endl;
      break;
    case ::rocksdb::log::kRecyclableMiddleType:
      std::cout << "kRecyclableMiddleType" << std::endl;
      break;
    case ::rocksdb::log::kRecyclableLastType:
      std::cout << "kRecyclableLastType" << std::endl;
      break;

    default:
    {
    }
    
  }*/
  return std::distance(beg, end);
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
    _buffer.erase( _buffer.begin(), _buffer.begin() + s);
  }
  return _status != status::Error;
}

since_reader::batch_ptr since_reader::detach()
{
  if ( _batch == nullptr )
    return std::make_unique<batch_type>();
  return std::move(_batch);
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
