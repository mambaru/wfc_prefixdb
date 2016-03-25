#pragma once

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>

#include <vector>
#include <string>

namespace wamba{ namespace prefixdb {
  
template<typename I, typename Out >
inline Out encode64(I beg, I end, Out out)
{
  using namespace boost::archive::iterators;
  typedef insert_linebreaks<base64_from_binary<transform_width<I, 6, 8> >, 72 > iterator;
  out = std::copy( iterator(beg), iterator(end), out);
  return std::fill_n( out, (3-std::distance(beg,end)%3)%3, '=');
}

// костыль для старого буста: tail - хвост который нужно отрезать в результате, модификация интервала
template<typename I, typename Out >
inline Out decode64(I beg, I end, Out out, size_t& tail)
{
  using namespace boost::archive::iterators;
  typedef transform_width<binary_from_base64< remove_whitespace<I> >, 8, 6> iterator;
  std::reverse_iterator<I> rend(end);
  for ( tail=0; tail < 2 && *rend == '='; ++tail, ++rend, *(end-tail)='A' );
  return std::copy( iterator(beg), iterator(end), out);
}
   
}}
