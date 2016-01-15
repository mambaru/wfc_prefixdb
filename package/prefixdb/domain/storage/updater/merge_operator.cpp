
#include <rocksdb/merge_operator.h>
#include <rocksdb/env.h>

#include "merge_operator.hpp"
#include "update.hpp"

#include <wfc/logger.hpp>
#include <wfc/json.hpp>

#include <algorithm>

namespace wamba{ namespace prefixdb{

namespace{
  using parser = ::wfc::json::parser;
  
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
}
  
bool merge_operator::Merge(const slice_type& key,
                     const slice_type* existing_value,
                     const slice_type& value,
                     std::string* new_value,
                     ::rocksdb::Logger* /*logger*/) const 
try
{
  DEBUG_LOG_MESSAGE("merge_operator::Merge: " << key.ToString()  )
  
  std::cout << value.ToString() << std::endl;
  update upd;
  update_json::serializer()( upd, value.data(), value.data() + value.size() );
  switch( upd.mode )
  {
    case update_mode::inc:
      this->inc_(*new_value, std::move(upd.value), begin(existing_value), end(existing_value) ); 
      break;
    case update_mode::packed:
      this->packed_(*new_value, std::move(upd.value), begin(existing_value), end(existing_value) ); 
      break;
    default: return false;
  }
  return true;
  /*
  if ( upd.mode == update_mode::inc)
  {
    this->inc_(*new_value, std::move(upd.value), begin(existing_value), end(existing_value) );
    return true;
  }
  return false;
  */
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
    packed_params_json::serializer()( upd, in.begin(), in.end() );
  }
  else
  {
    // LOG!!!
    out = std::move(in);
    return;
  }
  
  packed_t pck;
  
  if ( beg!=end && parser::is_object(beg, end) )
  {
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
  for (auto& u : upd )
  {
    //inc_params& u = p.second;
    field.first = std::move(u.key);
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
