#pragma once

#include <cstring>
#include <string>
#include "slicer.hpp"

namespace wamba{ namespace prefixdb{

template<typename T, typename Comp, typename DbCompare, typename SliceType >
class comparator
  : public DbCompare
{
  typedef comparator<T, Comp, DbCompare, SliceType> self;
public:
  typedef SliceType slice_type;
  typedef T value_type;
  typedef Comp comp_type;
public:
  
  comparator():_comp(){};
  comparator(const comp_type& comp):_comp(comp){};
  
  comparator(const self& ) = delete;
  self& operator=(const self& ) = delete;
  
  virtual int Compare(const slice_type& a, const slice_type& b) const 
  {
    const value_type& first = slice2value<value_type>(a);
    const value_type& second = slice2value<value_type>(b);
    if ( _comp(first, second ) ) return -1;
    if ( _comp(second, first ) ) return +1;
    return 0;
  };

  // The name of the comparator.  Used to check for comparator
  // mismatches (i.e., a DB created with one comparator is
  // accessed using a different comparator.
  //
  // The client of this package should switch to a new name whenever
  // the comparator implementation changes in a way that will cause
  // the relative ordering of any two keys to change.
  //
  // Names starting with "leveldb." are reserved and should not be used
  // by any clients of this package.
  virtual const char* Name() const 
  {
    return typeid(comp_type).name();
  }

  // Advanced functions: these are used to reduce the space requirements
  // for internal data structures like index blocks.

  // If *start < limit, changes *start to a short string in [start,limit).
  // Simple comparator implementations may return with *start unchanged,
  // i.e., an implementation of this method that does nothing is correct.
  virtual void FindShortestSeparator(
      std::string* /*start*/,
      const slice_type& /*limit*/) const
  {
    // разобраться и релизвать если нужно
  };

  // Changes *key to a short string >= *key.
  // Simple comparator implementations may return with *key unchanged,
  // i.e., an implementation of this method that does nothing is correct.
  virtual void FindShortSuccessor(std::string* /*key*/) const 
  {
    // разобраться и релизвать если нужно
  };

  const comp_type& basic() const
  {
    return _comp;
  }
private:
  comp_type _comp;
};

/*
template<typename Comp, typename Params>
class comparator< std::string, Comp, Params>
  : public Params::comparator_type
{
public:
  typedef Params super;
  typedef typename super::slice_type slice_type;
  typedef std::string value_type;
  typedef Comp comp_type;

public:
  comparator():_comp(){};
  comparator(const comp_type& comp):_comp(comp){};

  virtual int Compare(const slice_type& a, const slice_type& b) const override
  {
    int result = std::strncmp( a.data(), b.data(), std::min(a.size(), b.size() ) );
    if ( result!=0 ) return result;
    if ( a.size() > b.size() ) return 1;
    if ( a.size() < b.size() ) return -1;
    return 0;
  };

  virtual const char* Name() const override
  {
    return typeid(comp_type).name();
  }
  
  virtual void FindShortestSeparator(
      std::string* start,
      const slice_type& limit) const override
  {
  };

  virtual void FindShortSuccessor(std::string* key) const override
  { 
  };

  const comp_type& basic() const
  {
    return _comp;
  }

private:

  comp_type _comp;
  
};
*/

}}