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

#include <prefixdb/logger.hpp>
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
  inline bool unserialize(Obj& obj, I beg, I end, bool hide)
  {
    if ( beg==end )
      return false;
    typedef typename J::serializer ser;
    ::wfc::json::json_error e;
    ser()(obj, beg, end, &e);
    if ( e )
    {
      if (!hide)
        COMMON_LOG_ERROR( "unserialize merge_operator error: " << ::wfc::json::strerror::message_trace(e, beg, end) );
      return false;
    }
    return true;
  }

  template<typename J, typename Obj>
  inline bool unserialize(Obj& obj, const std::string& str, bool hide)
  {
    return unserialize<J>(obj, str.begin(), str.end() , hide);
  }

  template<typename J, typename Obj>
  inline bool unserialize(Obj& obj, const merge_operator::slice_type& slice, bool hide)
  {
    return unserialize<J>(obj, slice.data(), slice.data() + slice.size(), hide );
  }

  template<typename J, typename Obj>
  inline bool unserialize(Obj& obj, const merge_operator::slice_type* slice, bool hide)
  {
    if (slice==nullptr) return false;
    return unserialize<J>(obj, *slice, hide);
  }
}
  
merge_operator::merge_operator(size_t array_limit, size_t packed_limit)
 
{
  _array_limit = array_limit;
  _packed_limit = packed_limit;
}

bool merge_operator::FullMergeV2(
  const MergeOperationInput& merge_in,
  MergeOperationOutput* merge_out
) const
try
{
  PREFIXDB_LOG_DEBUG("bool merge_operator::FullMergeV2")
  const auto& operands = merge_in.operand_list;
  if ( operands.empty() )
    return true;
  
  merge mrg;
  merge_mode mode = merge_mode::none;
  
  update_list updates;
  updates.reserve(operands.size());
  for (size_t i = 0; i < operands.size(); ++i)
  {
    PREFIXDB_LOG_DEBUG("bool merge_operator::FullMergeV2 OPERAND: " << i)
    if ( helper::unserialize<merge_json>(mrg, operands[i], false) )
    {
      if ( mrg.mode == merge_mode::setnx && mode!=merge_mode::none )
        continue;
      
      if ( mode != mrg.mode )
      {
        // при i==0 или если сменился метод (тогда предыдущие не имеют смысла )
        updates.clear();
        mode = mrg.mode;
      }
      updates.push_back( std::move(mrg.raw) );
    }
    else
    {
      PREFIXDB_LOG_WARNING("Invalid merge operator: " << operands[i].ToString() )
    }
  } 
  
  merge_out->new_value.clear();
  
  PREFIXDB_LOG_DEBUG("bool merge_operator::FullMergeV2 switch...")
  switch( mode )
  {
    case merge_mode::setnx:
      this->setnx_(merge_in.existing_value,  updates, merge_out->new_value ); 
      break;
    case merge_mode::inc:
      this->inc_(merge_in.existing_value,    updates, merge_out->new_value ); 
      PREFIXDB_LOG_DEBUG("bool merge_operator::FullMergeV2 INC")
      break;
    case merge_mode::add:
      this->add_(merge_in.existing_value,    updates, merge_out->new_value ); 
      break;
    case merge_mode::packed:
      this->packed_(merge_in.existing_value, updates, merge_out->new_value ); 
      break;
    default:
      if ( !updates.empty() )
      {
        COMMON_LOG_WARNING("merge_operator::Merge: Invalid method merge: " << updates[0]  )
      }
      if ( merge_in.existing_value!=nullptr )
        merge_out->new_value = merge_in.existing_value->ToString();
      else
        merge_out->new_value="\"\"";
      COMMON_LOG_MESSAGE("merge_operator::Merge: Save old value: " << merge_out->new_value  )
  } // switch( mode )

  PREFIXDB_LOG_DEBUG("bool merge_operator::FullMergeV2 Done!")
  return true;
}
catch(std::exception e)
{
  if ( merge_in.existing_value )
    merge_out->new_value = merge_in.existing_value->ToString();
  
  DOMAIN_LOG_ERROR("PreffixDB merge_operator::FullMerge exception: "<< e.what() << ": key=" 
                  << merge_in.key.ToString() << " existing=" 
                  << ( merge_in.existing_value ? merge_in.existing_value ->ToString() : std::string("nullptr") )
                  << " operands=" << merge_in.operand_list.size() )
  return true;
}
catch(...)
{
  if ( merge_in.existing_value )
    merge_out->new_value = merge_in.existing_value->ToString();

  DOMAIN_LOG_ERROR("PreffixDB merge_operator::FullMerge unhandled exception: key=" 
                  << merge_in.key.ToString() << " existing=" 
                  << ( merge_in.existing_value ? merge_in.existing_value ->ToString() : std::string("nullptr") )
                  << " operands=" << merge_in.operand_list.size() )
  return true;
}

const char* merge_operator::Name() const 
{
  return "PreffixDBMergeOperator";
}

bool merge_operator::ShouldMerge(const std::vector<slice_type>& /*operands*/) const
{
  return true;
}

void merge_operator::setnx_(const slice_type* value, const update_list& operands, std::string& result) const
{
  if ( value!=nullptr )
  {
    result = value->ToString();
    return;
  }
  
  if ( operands.empty() )
    return;
  
  result = operands.front();
}

void merge_operator::inc_(const slice_type* value, const update_list& operands, std::string& result) const
{
  typedef ::wfc::json::value<int64_t> int64json;
  typedef ::wfc::json::value<int64_t>::serializer intser;
  int64_t num = 0;
  bool exist = true;
  // Десериализуем текущий объект
  if ( !helper::unserialize<int64json>(num, value, true)  )
  {
    num = 0;
    exist = false;
  }

  for (const std::string& oper : operands )
  {
    this->inc_operand_(oper, num, exist);
    exist = true;
  }

  result.reserve(8);
  intser()( num, std::inserter(result, result.end()) );
}

void merge_operator::inc_operand_(const std::string& oper, int64_t& num, bool exist) const
{
  typedef ::wfc::json::value<int64_t> int64json;
  inc_params params;

  if ( !helper::unserialize<inc_params_json>(params, oper, false) )
    return;
  
  if ( !exist )
  {
    if ( !parser::is_number( params.val.begin(), params.val.end() ) )
    {
      num = 0;
    }
    else
    {
      helper::unserialize<int64json>(num, params.val, false);
    }
  }
  
  if (parser::is_number( params.inc.begin(), params.inc.end()))
  {
    int64_t inc = 0;
    helper::unserialize<int64json>(inc, params.inc, false);
    num += inc;
  }
}

void merge_operator::add_(const slice_type* value, const update_list& operands, std::string& result) const
{
  std::deque<std::string> arr;
  typedef ::wfc::json::array< std::deque< ::wfc::json::raw_value<> > > arr_json;

  // Десериализуем текущий объект
  if ( !helper::unserialize<arr_json>(arr, value, false) )
  {
    // очевидно записан какой-то мусор похожий на объект. Не важно, просто заменим его
    arr.clear();
  }

  for (const std::string& oper : operands )
  {
    this->add_operand_(oper, arr);
  }
  
  if ( value!= nullptr )
    result.reserve(value->size());
  arr_json::serializer()( arr, std::inserter(result, result.end()) );
}

void merge_operator::add_operand_(const std::string& oper, std::deque<std::string>& arr) const
{
  add_params update;
  if ( !helper::unserialize<add_params_json>(update, oper, false) )
    return;
  
  if ( update.lim == 0 )
  {
    arr.clear();
    return;
  }

  arr.insert( arr.end(), update.arr.begin(), update.arr.end() );
  if ( arr.size() > update.lim )
  {
    size_t pos = arr.size() - update.lim;
    arr.erase( arr.begin(), arr.begin() + pos );
  }
}

void merge_operator::packed_(const slice_type* value, const update_list& operands, std::string& result) const
{
  packed_t pck;
  
  // Десериализуем текущий объект
  if ( !helper::unserialize<packed_json>(pck, value, false) )
  {
    // очевидно записан какой-то мусор похожий на объект. Не важно, просто заменим его
    pck.clear();
  }

  for (const std::string& oper : operands )
  {
    this->packed_operand_(oper, pck);
  }
  
  if ( value!= nullptr )
    result.reserve(value->size());
  packed_json::serializer()( pck, std::inserter(result, result.end()) );
}

void merge_operator::packed_operand_(const std::string& oper, packed_t& pck) const
{
  packed_params_t update;
  if ( !helper::unserialize<packed_params_json>(update, oper, false) )
    return;
    
  static auto less = []( const packed_field& l, const packed_field& r) { return l.first < r.first; };
  if ( !std::is_sorted( pck.begin(), pck.end(), less ) )
  { 
    // сортируем для быстропоиска
    // как фитча все поля у хранимого пакета упорядочены 
    std::sort( pck.begin(), pck.end(), less );
  }
    
  packed_field field;
  for (const auto& upd : update)
  {
    const packed_field_params& u = upd.second;
    const bool erase = parser::is_null( u.inc.begin(), u.inc.end() )
                    && parser::is_null( u.val.begin(), u.val.end() );

    field.first = std::move(upd.first);
    auto itr = std::lower_bound(pck.begin(), pck.end(), field, less);
    const bool found = ( itr != pck.end() && itr->first == field.first );

    if ( erase )
    {
      if ( found ) pck.erase(itr);
        continue;
    }

    // это инкремент или просто замена
    bool inc_ready = parser::is_number( u.inc.begin(), u.inc.end() );
    if ( !found )
    {
      field.second = u.val;
      itr = pck.insert(itr, std::move(field) );
    }

    if ( inc_ready )
      this->packed_inc_( u, itr->second );
    else if (found)
      itr->second = std::move( u.val );
  }
}

void merge_operator::packed_inc_(const packed_field_params& upd, std::string& result) const
{
  ::wfc::json::value<int64_t>::serializer intser;
  
  if ( !parser::is_number( result.begin(), result.end() ) )
  { 
    // если текущее значение не число, то берём из val
    result = upd.val;
    if ( !parser::is_number( result.begin(), result.end() ) )
    { // если все равно не число
      result = "0";
    }
  }
  
  int64_t val = 0;
  int64_t inc = 0;
  intser(val, result.begin(), result.end(), nullptr);
  intser(inc, upd.inc.begin(), upd.inc.end(), nullptr );
  val += inc;
  result.clear();
  
  // немного быстрее, чем std::back_inserter
  intser(val, std::inserter(result, result.end()) );
}

}}
