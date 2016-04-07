

#include "prefixdb_cmd.hpp"
#include <sstream>
#include <string>
#include <iostream>

namespace wamba{ namespace prefixdb{ namespace service{

namespace
{
  void delay_background_( std::shared_ptr<iprefixdb> db, std::stringstream& ss, ::wfc::iinterface::outgoing_handler_t handler)
  {
    time_t delay_s = 0;
    bool force = false;
    ss >> delay_s;
    ss >> force;
    auto req = std::make_unique<request::delay_background>();
    req->delay_timeout_s = delay_s;
    req->force = force;
    db->delay_background(std::move(req), [handler, delay_s, force](response::delay_background::ptr)
    {
      std::stringstream ss;
      ss << "OK. Background delayed on " << delay_s << " seconds. force=" << force;
      handler( ::iow::io::make(ss.str()) );
    });
  }

  void detach_prefixes_( std::shared_ptr<iprefixdb> db, std::stringstream& ss, ::wfc::iinterface::outgoing_handler_t handler)
  {
    auto req = std::make_unique<request::detach_prefixes>();
    time_t deny_timeout = 0;
    ss >> deny_timeout;
    req->deny_timeout_s = deny_timeout;
    
    std::string prefix;
    for (;;) 
    {
      ss >> prefix;
      if (prefix.empty()) break;
      req->prefixes.push_back(prefix);
    }
    
    db->detach_prefixes( std::move(req), [handler, deny_timeout](response::detach_prefixes::ptr)
    {
      std::stringstream ss;
      ss << "OK. Prefixes detached. Deny timeout " << deny_timeout << " seconds. ";
      handler( ::iow::io::make(ss.str()) );
    } );
  }

  void get_all_prefixes_( std::shared_ptr<iprefixdb> db, std::stringstream& , ::wfc::iinterface::outgoing_handler_t handler)
  {
    auto req = std::make_unique<request::get_all_prefixes>();
    db->get_all_prefixes( std::move(req), [handler](response::get_all_prefixes::ptr res)
    {
      std::stringstream ss;
      if ( res!=nullptr ) for (const std::string& preffix: res->prefixes )
      {
        ss << preffix << std::endl;
      }
      ss << "OK";
      handler( ::iow::io::make(ss.str()) );
    } );
  }
  
  const char* help_str[][4] = { 
    {"", "help", "[<<command>>]",  "Подсказка по конкретной команде. Если не указана, то список всех комманд."}, 
    {"db", "delay_background", "[0](delay seconds) [0](force)", "Завершает все фоновые процессы на delay_s секунд. \n"
                               "Если force=1 то по завершению таймаута гарантировано запустит все фоновые \n"
                               "процессы, даже если в это время были вызовы delay_background c большим таймаутом. "
                               "Для гаранитированного запуска используй: 'db 0 1'"},
    {"dp", "detach_prefixes",  "[0](access denied in sec) <<prefix1>> [<<prefix2>> ...]", "Отсоединяет префиксы на заданный таймаут. База префиксов перемещаеться в указанное \n"
                               "в конфигурации место. Префикс станет доступен через заданное первым параметром число секунд." },
    {"gap", "get_all_prefixes",  "", "Получить спискок всех доступных префиксов" }

  };
  void help_( std::shared_ptr<iprefixdb> /*db*/, std::stringstream& ss, ::wfc::iinterface::outgoing_handler_t handler)
  {
    std::stringstream oss;
    std::string cmd;
    ss >> cmd;
    for (auto hlp: help_str)
    {
      //std::cout << cmd << ":" << std::string(hlp[0]) << ":" << std::string(hlp[1]) << std::endl;
      if ( cmd.empty() || cmd==std::string(hlp[0]) || cmd==std::string(hlp[1]) )
      {
        if ( !std::string(hlp[0]).empty() )
          oss << hlp[0] << "|";
        oss << hlp[1] << " " << hlp[2] << std::endl;
        if ( !cmd.empty() )
        {
          oss << hlp[3] << std::endl;
        }
      }
    }
    auto res = oss.str();
    res.resize(res.size() - 1);
    handler( ::iow::io::make( res) );
  }

}
  
void prefixdb_cmd( std::shared_ptr<iprefixdb> db, ::wfc::iinterface::data_ptr d, ::wfc::iinterface::outgoing_handler_t handler)
{
  std::string result;
  std::stringstream ss;
  ss << std::string( d->begin(), d->end() );
  std::string method;
  ss >> method;
  //std::cout << "method=" << method  << std::endl;
  if ( method == "help")
  {
    help_(db, ss, std::move(handler) );
  }
  else if ( method == "db" || method=="delay_background")
  {
    delay_background_(db, ss, std::move(handler) );
  }
  else if ( method == "dp" || method=="detach_prefixes")
  {
    detach_prefixes_(db, ss, std::move(handler) );
  }
  else if ( method == "gap" || method=="get_all_prefixes")
  {
    get_all_prefixes_(db, ss, std::move(handler) );
  }
  else
  {
    handler( ::iow::io::make("Unknown method") );
  }
  
}

}}}
