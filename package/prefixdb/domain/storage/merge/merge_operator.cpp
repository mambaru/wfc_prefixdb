#include "merge_operator.hpp"
#include "merge.hpp"
#include "merge_json.hpp"

#include "inc_params.hpp"
#include "inc_params_json.hpp"

#include "packed_params.hpp"
#include "packed_params_json.hpp"

#include "packed.hpp"
#include "packed_json.hpp"

#include <wfc/logger.hpp>
#include <wfc/json.hpp>
#include <algorithm>

namespace wamba{ namespace prefixdb{

using parser = ::wfc::json::parser;
namespace helper{
  
  inline const char* begin(const merge_operator::slice_type* existing_value)
  {
    if ( existing_value==nullptr ) 
      return nullptr;
    return existing_value->data();
  }

  inline const char* end(const merge_operator::slice_type* existing_value)
  {
    if ( existing_value==nullptr ) 
      return nullptr;
    return existing_value->data() + existing_value->size();
  }
  
  template<typename J, typename Obj, typename I>
  inline bool unserialize(Obj& obj, I beg, I end)
  try
  {
    if ( beg==end )
      return false;
    typedef typename J::serializer ser;
    ser()(obj, beg, end);
    return true;
  }
  catch( const ::wfc::json::json_error& e)
  {
    e.message(beg, end);
    return false;
  }

  template<typename J, typename Obj>
  inline bool unserialize(Obj& obj, const std::string& str)
  {
    return unserialize<J>(obj, str.begin(), str.end() );
  }

  template<typename J, typename Obj>
  inline bool unserialize(Obj& obj, const merge_operator::slice_type& slice)
  {
    return unserialize<J>(obj, slice.data(), slice.data() + slice.size() );
  }

  template<typename J, typename Obj>
  inline bool unserialize(Obj& obj, const merge_operator::slice_type* slice)
  {
    if (slice==nullptr) return false;
    return unserialize<J>(obj, *slice);
  }

  
  
}
  
bool merge_operator::Merge(const slice_type& key,
                     const slice_type* existing_value,
                     const slice_type& value,
                     std::string* new_value,
                     ::rocksdb::Logger* /*logger*/) const 
try
{
  DEBUG_LOG_MESSAGE("merge_operator::Merge: " << value.ToString()  )
  
  merge upd;
  helper::unserialize<merge_json>(upd, value);
  std::cout << int(upd.mode) << std::endl;
  switch( upd.mode )
  {
    case merge_mode::inc:
      std::cout << "M1" << std::endl;
      this->inc_(*new_value, std::move(upd.raw), helper::begin(existing_value), helper::end(existing_value) ); 
      break;
    case merge_mode::packed:
      std::cout << "M2" << std::endl;
      this->packed_(*new_value, std::move(upd.raw), helper::begin(existing_value), helper::end(existing_value) ); 
      break;
    default: 
      std::cout << "M3" << std::endl;
      return false;
  }
  return true;
}
catch(std::exception e)
{
  if ( existing_value )
    *new_value = existing_value->ToString();
  
  DOMAIN_LOG_ERROR("PreffixDB merge_operator::Merge exception: "<< e.what() << ": key=" 
                  << key.ToString() << " existing=" << ( existing_value ? existing_value->ToString() : std::string("nullptr") )
                  << " value=" << value.ToString() )
  return true;
}
catch(...)
{
  if ( existing_value )
    *new_value = existing_value->ToString();

  DOMAIN_LOG_ERROR("PreffixDB merge_operator::Merge unhandled exception: key=" 
                  << key.ToString() << " existing=" << ( existing_value ? existing_value->ToString() : "nullptr" )
                  << " value=" << value.ToString() )
  return true;
}
  
const char* merge_operator::Name() const 
{
  return "PreffixDBMergeOperator";
}


void merge_operator::inc_(std::string& out, std::string&& upd, const char* beg, const char* end ) const
{
  ::wfc::json::value<int64_t>::serializer intser;

  inc_params params;
  if ( parser::is_object(upd.begin(), upd.end()) )
  {
    inc_params_json::serializer()( params, upd.begin(), upd.end() );
  }
  else
  {
    // LOG!!!
    out = std::move(params.val);
    return;
  }
  
  if ( beg!=end 
    && parser::is_number(beg, end)
    && parser::is_number( params.inc.begin(), params.inc.end() )
  )
  { 
    int64_t val = 0;
    intser(val, beg, end);
    
    int64_t inc = 0;
    intser(inc, params.inc.begin(), params.inc.end());
    
    val += inc;
    intser(val, std::inserter(out, out.end()) );
  }
  else
  {
    out = std::move(params.val);
  }
}

void merge_operator::packed_(std::string& out, std::string&& in, const char* beg, const char* end ) const
{
  packed_params_t upd;
  if ( parser::is_object(in.begin(), in.end()) )
  {
    std::cout << in << std::endl;
    packed_params_json::serializer()( upd, in.begin(), in.end() );
    //std::cout << "upd[0].key=" << upd[0].key << std::endl;
  }
  else
  {
    std::cout << "-3-" << std::endl;
    // LOG!!!
    out = std::move(in);
    return;
  }
  
  packed_t pck;
  
  if ( beg!=end && parser::is_object(beg, end) )
  {
    std::cout << "-4-" << std::endl;
    try{
      packed_json::serializer()(pck, beg, end);
    } catch(...) { /* очевидно записан какой-то мусор похожий на объект. Не важно, просто заменим его */ }
  }
  
  static auto less = []( const packed_field& l, const packed_field& r) { return l.first < r.first; };

  if ( !std::is_sorted( pck.begin(), pck.end() ) )
  { 
    // сортируем для быстропоиска
    // как фитча все поля у хранимого пакета упорядочены 
    std::sort( pck.begin(), pck.end(), less );
  }

  packed_field field;
  for (auto& p : upd )
  {
    packed_field_params& u = p.second;
    //std::cout << ">>> " << u.key << " " << u.inc << std::endl;
    //inc_params& u = p.second;
    field.first = std::move(p.first);
    auto itr = std::lower_bound(pck.begin(), pck.end(), field, less);

    if ( parser::is_null( u.inc.begin(), u.inc.end() ) 
      && parser::is_null( u.val.begin(), u.val.end() ) 
    )
    { 
      // Если не заданы оба, то удаляем поле
      if ( itr != pck.end() )
        pck.erase(itr);
    }
    else
    {
      bool inc_ready = parser::is_number( u.inc.begin(), u.inc.end() );
      
      if ( itr == pck.end() || itr->first != field.first )
      { 
        field.second = inc_ready ? "0" : "null" ;
        itr = pck.insert(itr, std::move(field) );
      }
      
      if ( inc_ready )
      { // если в inc число, то работаем как с числом и делаем инкремент
        
        ::wfc::json::value<int64_t>::serializer intser;
        if ( !parser::is_number( itr->second.begin(), itr->second.end() ) )
        { // если текущее значение не число, то берём из val
          itr->second = std::move( u.val );
          if ( !parser::is_number( itr->second.begin(), itr->second.end() ) )
          { // если все равно не число
            itr->second = "0";
          }
        }
        
        int64_t val = 0;
        int64_t inc = 0;
        intser(val, itr->second.begin(), itr->second.end());
        intser(inc, u.inc.begin(), u.inc.end() );
        val += inc;
        itr->second.clear();
        // немного быстрее, чем std::back_inserter
        intser(val, std::inserter(itr->second, itr->second.end()) );
      }
      else
      {
        itr->second = std::move(u.val);
      }
    }
  }
  
  out.reserve(in.size());
  packed_json::serializer()( pck, std::inserter(out, out.end()) );
}

/*
bool merge_operator::set_(const slice_type& key,
                          const slice_type* existing_slice,
                          const slice_type& value_op,
                          std::string* new_value,
                          ::rocksdb::Logger* logger) const
{
  return true;
}

bool merge_operator::inc_(const slice_type& key,
                          const slice_type* existing_slice,
                          const slice_type& value_op,
                          std::string* new_value,
                          ::rocksdb::Logger* logger) const
{
  return true;
}

bool merge_operator::upd_(const slice_type& key,
                          const slice_type* existing_slice,
                          const slice_type& value_op,
                          std::string* new_value,
                          ::rocksdb::Logger* logger) const
{
  return true;
}
*/

}}
