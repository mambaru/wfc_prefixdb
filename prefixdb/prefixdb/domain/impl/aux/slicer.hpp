#pragma once

#include <memory>
#include <fas/typemanip.hpp>

namespace wamba{ namespace prefixdb{

namespace helper
{
  template<typename SliceType>
  inline std::string slice2value(const SliceType& value, fas::type2type<std::string>)
  {
    return std::move(value.ToString());
  }

  template<typename ValueType, typename SliceType>
  inline ValueType slice2value(const SliceType& value, fas::type2type<ValueType>)
  {
    return *reinterpret_cast<const ValueType*>(value.data());
  }

  inline std::string string2value(const std::string& value, fas::type2type<std::string>)
  {
    return std::move(value);
  }

  template<typename ValueType>
  inline ValueType string2value(const std::string& value, fas::type2type<ValueType>)
  {
    return *reinterpret_cast<const ValueType*>(value.data());
  }

}

template<typename ValueType, typename SliceType>
inline ValueType slice2value(const SliceType& value)
{
  return helper::slice2value(value, fas::type2type<ValueType>() );
}

template<typename ValueType>
inline ValueType string2value(const std::string value)
{
  return helper::string2value(value, fas::type2type<ValueType>() );
}


template<typename SliceType, typename ValueType>
inline SliceType value2slice(const ValueType& value)
{
  return SliceType( reinterpret_cast<const char*>(&value), sizeof(value) );
}

template<typename SliceType>
inline SliceType value2slice(const std::string& value)
{
  return SliceType( value );
}

}}