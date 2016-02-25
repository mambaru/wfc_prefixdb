#include "merge_operator.hpp"
#include "merge.hpp"
#include "merge_json.hpp"

#include "inc_params.hpp"
#include "inc_params_json.hpp"

#include "packed_params.hpp"
#include "packed_params_json.hpp"

#include "add_params.hpp"
#include "add_params_json.hpp"

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
    COMMON_LOG_ERROR( "unserialize merge_operator error: " << e.message(beg, end) );
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
  
merge_operator::merge_operator()
{
  _config = std::make_shared<merge_config>();
}

void merge_operator::reconfigure(const merge_config& config)
{
  _config = std::make_shared<merge_config>(config);
}

bool merge_operator::FullMerge(
    const slice_type& key,
    const slice_type* value,
    const operand_list& operands,
    std::string* result,
    logger_type* logger) const
try
{
  if ( operands.empty() )
    return true;
  
  merge mrg;
  merge_mode mode = merge_mode::none;
  
  update_list updates;
  updates.reserve(operands.size());
  for (size_t i = 0; i < operands.size(); ++i)
  {
    if ( helper::unserialize<merge_json>(mrg, operands[i]) )
    {
      if ( mode == merge_mode::none )
        mode = mrg.mode;
      updates.push_back(mrg.raw);
    }
  } 
  
  switch( mode )
  {
    /*
    case merge_mode::inc:
      this->inc_(*new_value, std::move(upd.raw), beg, end ); 
      break;
    case merge_mode::add:
      this->add_(*new_value, std::move(upd.raw), beg, end ); 
      break;
      */
    case merge_mode::packed:
      this->packed_(value, updates, *result); 
      break;
    default: 
      COMMON_LOG_MESSAGE("merge_operator::Merge: Invalid method merge: " << updates[0]  )
      if ( value!=nullptr )
        *result = value->ToString();
      else
        *result="\"\"";
      COMMON_LOG_MESSAGE("merge_operator::Merge: Save old value: " << *result  )
  }
  DEBUG_LOG_MESSAGE("Merge save: " 
    << "\nValue: "   << (value ? value->ToString() : "nullptr")
    << "\nOperand[0]: " << updates[0]
    << "\nResult: " << *result)
  return true;

}
catch(std::exception e)
{
  if ( value )
    *result = value->ToString();
  
  DOMAIN_LOG_ERROR("PreffixDB merge_operator::FullMerge exception: "<< e.what() << ": key=" 
                  << key.ToString() << " existing=" << ( value ? value->ToString() : std::string("nullptr") )
                  << " operands=" << operands.size() )
  return true;
}
catch(...)
{
  if ( value )
    *result = value->ToString();

  DOMAIN_LOG_ERROR("PreffixDB merge_operator::FullMerge unhandled exception: key=" 
                  << key.ToString() << " value=" << ( value ? value->ToString() : "nullptr" )
                  << " operands=" << operands.size() )
  return true;
}



bool merge_operator::Merge(const slice_type& key,
                     const slice_type* existing_value,
                     const slice_type& value,
                     std::string* new_value,
                     ::rocksdb::Logger* /*logger*/) const 
try
{
  merge upd;
  helper::unserialize<merge_json>(upd, value);
  
  const char *beg = helper::begin(existing_value);
  const char *end = helper::end(existing_value);
  switch( upd.mode )
  {
    case merge_mode::inc:
      this->inc_(*new_value, std::move(upd.raw), beg, end ); 
      break;
    case merge_mode::add:
      this->add_(*new_value, std::move(upd.raw), beg, end ); 
      break;
    case merge_mode::packed:
      this->packed_(*new_value, std::move(upd.raw), beg, end ); 
      break;
    default: 
      COMMON_LOG_MESSAGE("merge_operator::Merge: Invalid method merge: " << value.ToString()  )
      if ( beg!=end )
        new_value->assign(beg, end );
      else
        *new_value="\"\"";
      COMMON_LOG_MESSAGE("merge_operator::Merge: Save old value: " << *new_value  )
  }
  DEBUG_LOG_MESSAGE("Merge save: " 
    << "\nValue: "   << (beg!=end ? std::string(beg, end) : "nullptr")
    << "\nOperand: " << value.ToString()
    << "\nResult: " << *new_value)
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
  
  bool ready = false;
  if ( parser::is_object(upd.begin(), upd.end()) )
  {
    try{
      inc_params_json::serializer()( params, upd.begin(), upd.end() );
      ready = true;
    } catch(...) {}
  }
  
  if ( !ready )
  {
    if (beg!=end) out.assign(beg, end);
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
    if ( parser::is_number( params.val.begin(), params.val.end() ) )
    {
      out = std::move(params.val);
    }
    else
    {
      out = "0";
    }
  }
}

void merge_operator::add_(std::string& out, std::string&& in, const char* beg, const char* end ) const
{
  add_params upd;
  bool ready = false;
  if ( parser::is_object(in.begin(), in.end()) )
  {
    try{
      add_params_json::serializer()( upd, in.begin(), in.end() );
    } catch(...) {};
    ready = true;
  }
  
  if ( !ready )
  {
    if (beg!=end) 
    {
      out.assign(beg, end);
    }
    return;
  }
  
  if ( upd.lim == 0 )
  {
    out="[]";
    return;
  }
  
  std::vector<std::string> arr;
  typedef ::wfc::json::array< std::vector< ::wfc::json::raw_value<> > > arr_json;
  
  if (parser::is_array(beg, end) )
  {
    try{
      arr_json::serializer()(arr, beg, end);
    } catch(...) { /* очевидно записан какой-то мусор похожий на объект. Не важно, просто заменим его */ }
  }
  
  if ( parser::is_array(upd.arr.begin(), upd.arr.end()) )
  {
    try{
      std::vector<std::string> tail;
      tail.reserve(upd.lim);
      arr_json::serializer()(tail, upd.arr.begin(), upd.arr.end());
      arr.insert( arr.end(), tail.begin(), tail.end());
      //arr.emplace_back(tail.begin(), tail.end());
    } catch(...) { /* очевидно записан какой-то мусор похожий на объект. Не важно, просто заменим его */ }
  }
  else
  {
    // Добавлем объект как он пришел
    arr.push_back(upd.arr);
  }
  
  if ( arr.size() > upd.lim )
  {
    size_t pos = arr.size() - upd.lim;
    arr.erase( arr.begin(), arr.begin() + pos );
  }
  
  out.reserve( upd.arr.size() + std::distance(beg, end) );
  arr_json::serializer()(arr, std::inserter(out, out.end()));
}

void merge_operator::packed_(const slice_type* value, const update_list& operands, std::string& result) const
{
  packed_t pck;
  // Десериализуем текущий объект
  if ( value != nullptr )
  {
    const char* beg = value->data();
    const char* end = beg + value->size();
    if ( parser::is_object(beg, end) ) try
    {
      packed_json::serializer()(pck, beg, end);
    } catch(...) { /* очевидно записан какой-то мусор похожий на объект. Не важно, просто заменим его */ }
  }

  //for (auto )
  
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
    //out = std::move(in);
    if (beg!=end) out.assign(beg, end);
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
  for (auto& p : upd )
  {
    packed_field_params& u = p.second;
    const bool erase = parser::is_null( u.inc.begin(), u.inc.end() )
                    && parser::is_null( u.val.begin(), u.val.end() );
              
    field.first = std::move(p.first);
    auto itr = std::lower_bound(pck.begin(), pck.end(), field, less);
    const bool found = ( itr != pck.end() && itr->first == field.first );

    if ( erase )
    {
      if ( found ) pck.erase(itr);
      continue;
    }
    
    bool inc_ready = parser::is_number( u.inc.begin(), u.inc.end() );
      
    if ( !found )
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
    
  } // for
  
  out.reserve(in.size());
  packed_json::serializer()( pck, std::inserter(out, out.end()) );
}

}}
