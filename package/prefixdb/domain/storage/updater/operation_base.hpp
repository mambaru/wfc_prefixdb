#pragma once

#include <prefixdb/api/aux/field_type.hpp>
#include <functional>

namespace wamba{ namespace prefixdb{
 
enum class operation
{
  set,
  inc,
  upd
};

template<operation Op>
struct operation_base
{
private:
  operation op = Op;
public:
  field_type type = field_type::any; // number, string, package, any, none - запрещено
  time_t ttl = 0;
  bool force = true;

  /*
  typedef std::function<void(std::string key, std::string val, field_type type, time_t ttl)> handler_fun;
  handler_fun* handler = nullptr;
  */
  
  typedef std::vector<char> buffer_type;
};

}}