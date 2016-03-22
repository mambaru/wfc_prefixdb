
#include "since_reader.hpp"
#include "db/log_format.h"
#include <iostream>
#include <wfc/logger.hpp>

namespace wamba{ namespace prefixdb {

namespace{
inline void get_size(const char ** beg, size_t& size, int n = 0)
{
  size_t cur = static_cast<size_t>( *reinterpret_cast<const uint8_t*>(*beg) );
  (*beg)++;
  DEBUG_LOG_MESSAGE( "CUR=" << cur)
  
  bool fin = ((cur >> 7) & 1) == 0;
  
  if ( !fin )
  {
    cur &= ~(1 << 7);
    get_size(beg, size, n + 1);
  }
  if ( n > 0 )
    cur <<= 7*n;
  size |= cur;
  DEBUG_LOG_MESSAGE( "BIG Size=" << size << " cur=" << cur <<  " n=" << n)
  return;
    
  
  
  size_t tmp = cur;
  
  (*beg)++;
  
  //bool fin = ((cur >> 7) & 1) == 0;
  
  // Переносим младший бит в size
  if ( n!=0 && (cur & 1) ) 
    size |= 1 << ( (n-1)*8 + 8 - n );
  
  if ( !fin )
    cur &= ~(1 << 7);
  
  // Перонсим байт в size
  if ( n == 0 )
  {
    size = cur;
  }
  else
  {
    cur >>= 1;
    cur <<= 8*n;
    size |= cur;
  }
  
  
  if ( !fin ) {
    DEBUG_LOG_MESSAGE( "NEXT Size=" << size << " cur=" << tmp << " n=" << n)
    get_size( beg, size, n + 1);
  }
  
  DEBUG_LOG_MESSAGE( "BIG Size=" << size << " cur=" << tmp<< " n=" << n)
  if ( n != 0) abort();
}

inline size_t get_size(const char ** beg)
{
  size_t size = 0;
  get_size(beg, size);
  DEBUG_LOG_MESSAGE( "BIG Size=" << size )
  if (size > 127 )
    abort();
  return size;
  /*
  size_t size = static_cast<size_t>( *reinterpret_cast<const uint8_t*>(*beg) );
  (*beg)++;
  if ( size & 0xFF )
  {
    size &= ~(1 << 7);
    size_t size2 = static_cast<size_t>( *reinterpret_cast<const uint8_t*>(*beg) );
    if ( size2 & 1 ) size |= 1 << 7;
    size2 >>= 1;
    size2 &= ~(1 << 7);
    size2 <<= 8;
    size |= size2;
    DEBUG_LOG_MESSAGE( "BIG Size=" << size )
    abort();
  }
  DEBUG_LOG_MESSAGE( "Get Size=" << size )
  return size;
  */
}
}
  
const char*  since_reader::read_put_(const char* beg, const char* end)
{
  //int size = int( *reinterpret_cast<const uint8_t*>(beg++) );
  size_t size = get_size(&beg);
  std::cout << "Put " << size << ":" << int(*beg) << ":" << std::string(beg, beg + size) << "=";
  beg += size;
  size = get_size(&beg);
  std::cout << size << ":" << int(*beg) << ":" << std::string(beg, beg + size) ;
  beg += size;
  std::cout<< std::endl;
  return beg;
}

const char*  since_reader::read_del_(const char* beg, const char* end)
{
  int size = get_size(&beg);
  std::cout << "Del " << size << ":" << std::string(beg, beg + size) << std::endl;
  return beg + size;
}

const char*  since_reader::read_merge_(const char* beg, const char* end)
{
  int size = get_size(&beg);
  std::cout << "Merge " << size << ":" << int(*beg) << ":" << std::string(beg, beg + size) << "=";
  beg += size;
  size = get_size(&beg);
  std::cout << size << ":" << int(*beg) << ":" << std::string(beg, beg + size) ;
  beg += size;
  std::cout<< std::endl;
}

const char*  since_reader::read_item_(const char* beg, const char* end)
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
      beg--;
      std::cout << "UNKNOWN TYPE size=" << std::distance(beg, end)  << ": "  << int(*(beg-1)) << "," << int(*beg) << "," << int(beg[1]) << std::endl;
      for ( size_t i = 0; i != std::distance(beg, end); i++)
        std::cout << int(beg[i]) << " ";
      std::cout << std::endl;
      for ( size_t i = 0; i != std::distance(beg, end); i++)
        std::cout << char(beg[i]);
      std::cout << std::endl;
      beg = end;
  };
  
  if ( beg==end )
  {
    std::cout << "end" << std::endl;
    return nullptr;
  }
  return beg!=end ? beg : nullptr;
}

unsigned int since_reader::read_record_(const char *beg, const char *end)
{
  const unsigned int type = beg[4];
  size_t head = 12;
  for (int i =0 ; i < head; ++i)
    std::cout << int(beg[i]) << " ";
  std::cout << "| " << std::distance(beg,end) << ":";
  std::cout << std::endl;
  beg += head;
  while ( beg = this->read_item_(beg, end) );
  /*
  for ( beg+=head ; beg!=end; ++beg)
    std::cout << *beg;
  std::cout << std::endl;
  */
  return type;
}

size_t since_reader::parse_()
{
  const char *beg = &(_buffer[0]);
  const char *end = beg + _buffer.size();
  
  const unsigned int record_type = read_record_(beg, end);
  switch ( record_type )
  {
  // Zero is reserved for preallocated files
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
  }
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

since_reader::item_info since_reader::pop()
{
  item_info itm = std::move(_item_list.front());
  _item_list.pop_front();
  return std::move(itm);
}

since_reader::batch_ptr since_reader::pop_batch()
{
  return std::move(_batch);
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
