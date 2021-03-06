

#include "prefixdb_cmd.hpp"
#include "../api/common_status_json.hpp"
#include <sstream>
#include <string>
#include <iostream>

namespace wamba{ namespace prefixdb{ namespace service{

namespace
{
  void delay_background_( std::shared_ptr<iprefixdb> db, std::stringstream& ss, wfc::iinterface::output_handler_t handler)
  {
    time_t delay_s = 0;
    bool force = false;
    ss >> delay_s;
    ss >> force;
    auto req = std::make_unique<request::delay_background>();
    req->delay_timeout_s = delay_s;
    req->contunue_force = force;
    std::string prefix;
    for (;;)
    {
      prefix.clear();
      ss >> prefix;
      if (prefix.empty()) break;
      req->prefixes.push_back(prefix);
    }

    db->delay_background(std::move(req), [handler, delay_s, force](response::delay_background::ptr res)
    {
      std::stringstream ss2;
      ss2 << res->status << ". Backgrounds delayed on " << delay_s << " seconds. force=" << force;
      handler( iow::io::make(ss2.str()) );
    });
  }

  void continue_background_( std::shared_ptr<iprefixdb> db, std::stringstream& ss, wfc::iinterface::output_handler_t handler)
  {
    bool force = false;

    ss >> force;
    auto req = std::make_unique<request::continue_background>();
    req->force = force;
    std::string prefix;
    for (;;)
    {
      prefix.clear();
      ss >> prefix;
      if (prefix.empty()) break;
      req->prefixes.push_back(prefix);
    }

    db->continue_background(std::move(req), [handler, force](response::continue_background::ptr res)
    {
      std::stringstream ss2;
      ss2 << res->status << ". Continue backgrounds. force=" << force;
      handler( iow::io::make(ss2.str()) );
    });
  }

  void detach_prefixes_( std::shared_ptr<iprefixdb> db, std::stringstream& ss, wfc::iinterface::output_handler_t handler)
  {
    auto req = std::make_unique<request::detach_prefixes>();
    time_t deny_timeout = 0;
    ss >> deny_timeout;
    req->deny_timeout_s = deny_timeout;

    std::string prefix;
    for (;;)
    {
      prefix.clear();
      ss >> prefix;
      if (prefix.empty()) break;
      req->prefixes.push_back(prefix);
    }

    db->detach_prefixes( std::move(req), [handler, deny_timeout](response::detach_prefixes::ptr res)
    {
      std::stringstream ss2;
      ss2 << res->status << ". Prefixes detached. Deny timeout " << deny_timeout << " seconds. ";
      handler( iow::io::make(ss2.str()) );
    } );
  }

  void attach_prefixes_( std::shared_ptr<iprefixdb> db, std::stringstream& ss, wfc::iinterface::output_handler_t handler)
  {
    auto req = std::make_unique<request::attach_prefixes>();
    int opendb = 0;
    ss >> opendb;
    req->opendb = opendb!=0;

    std::string prefix;
    for (;;)
    {
      prefix.clear();
      ss >> prefix;
      if (prefix.empty()) break;
      req->prefixes.push_back(prefix);
    }

    db->attach_prefixes( std::move(req), [handler](response::attach_prefixes::ptr res)
    {
      std::stringstream ss2;
      ss2 << res->status << ". Prefixes attached.";
      handler( iow::io::make(ss2.str()) );
    } );
  }

  void get_all_prefixes_( std::shared_ptr<iprefixdb> db, std::stringstream& , wfc::iinterface::output_handler_t handler)
  {
    auto req = std::make_unique<request::get_all_prefixes>();
    db->get_all_prefixes( std::move(req), [handler](response::get_all_prefixes::ptr res)
    {
      std::stringstream ss;
      if ( res!=nullptr ) for (const std::string& preffix: res->prefixes )
      {
        ss << preffix << std::endl;
      }
      ss << res->status;
      handler( ::iow::io::make(ss.str()) );
    } );
  }

  void get_( std::shared_ptr<iprefixdb> db, std::stringstream& ss, wfc::iinterface::output_handler_t handler)
  {
    auto req = std::make_unique<request::get>();
    ss >> req->prefix;
    while (ss)
    {
      std::string name;
      ss >> name;
      if ( !name.empty() )
        req->fields.push_back(name);
    }
    db->get( std::move(req), [handler](response::get::ptr res)
    {
      std::stringstream oss;
      if ( res!=nullptr ) for (const auto& field: res->fields )
      {
        oss << field.first << "=" << field.second << std::endl;
      }
      oss << res->status;
      handler( ::iow::io::make(oss.str()) );
    } );
  }

  void del_( std::shared_ptr<iprefixdb> db, std::stringstream& ss, wfc::iinterface::output_handler_t handler)
  {
    auto req = std::make_unique<request::del>();
    ss >> req->prefix;
    while (ss)
    {
      std::string name;
      ss >> name;
      if ( !name.empty() )
        req->fields.push_back(name);
    }
    req->nores = false;
    req->noval = false;

    db->del( std::move(req), [handler](response::del::ptr res)
    {
      std::stringstream oss;
      if ( res!=nullptr ) for (const auto& field: res->fields )
      {
        oss << field.first << "=" << field.second << std::endl;
      }
      oss << res->status;
      handler( ::iow::io::make(oss.str()) );
    } );
  }

  void set_( std::shared_ptr<iprefixdb> db, std::stringstream& ss, wfc::iinterface::output_handler_t handler)
  {
    std::string prefix, key, value;
    ss >> prefix;
    ss >> key;
    value.assign(std::istreambuf_iterator<char>(ss), {});
    bool ready = false;
    if ( prefix.empty() )
      handler( iow::io::make("Error: Prefix required") );
    else if ( key.empty() )
      handler( iow::io::make("Error: Key required") );
    else
    {
      ready=true;
      wjson::json_error e;
      auto beg = wjson::parser::parse_space(value.begin(), value.end(), nullptr);
      value.erase(value.begin(), beg);
      wjson::parser::parse_value(beg, value.end(), &e);
      if ( e )
      {
        std::string newval;
        wjson::value<std::string>::serializer()( value, std::back_inserter(newval) );
        value.swap(newval);
      }
    }

    if (  !ready  )
      return;

    auto req = std::make_unique<request::set>();
    req->prefix = prefix;
    req->fields.push_back(std::make_pair(key, value));

    db->set( std::move(req), [handler](response::set::ptr )
    {
      handler( ::iow::io::make("OK") );
    } );
  }

  void corrupt_( std::shared_ptr<iprefixdb> db, std::stringstream& ss, wfc::iinterface::output_handler_t handler)
  {
    std::string prefix, key, value;
    ss >> prefix;
    ss >> key;
    ss >> value;
    bool ready = false;
    if ( prefix.empty() )
      handler( iow::io::make("Error: Prefix required") );
    else if ( key.empty() )
      handler( iow::io::make("Error: Key required") );
    else
      ready=true;

    if (  !ready  )
      return;

    auto req = std::make_unique<request::set>();
    req->prefix = prefix;
    req->fields.push_back(std::make_pair(key, value));

    db->set( std::move(req), [handler](response::set::ptr )
    {
      handler( ::iow::io::make("OK") );
    } );
  }

  void inc_( std::shared_ptr<iprefixdb> db, std::stringstream& ss, wfc::iinterface::output_handler_t handler)
  {

    auto req = std::make_unique<request::inc>();
    ss >> req->prefix;
    req->fields.resize(1);
    std::string key = "null";
    std::string inc = "null";
    std::string val = "null";
    ss >> key >> inc >> val;
    std::stringstream param;
    param << "{\"inc\":"<< inc << ",\"val\":" << val << "}";
    req->fields.back().first = key;
    req->fields.back().second = param.str();

    db->inc( std::move(req), [handler](response::inc::ptr )
    {
      handler( ::iow::io::make("OK") );
    } );

  }

  void compact_prefix_( std::shared_ptr<iprefixdb> db, std::stringstream& ss, wfc::iinterface::output_handler_t handler)
  {
    auto req = std::make_unique<request::compact_prefix>();
    ss >> req->prefix;
    if (ss) ss >> req->from;
    if (ss) ss >> req->to;
    db->compact_prefix( std::move(req), [handler](response::compact_prefix::ptr res)
    {
      if ( res->status == common_status::OK )
        handler( ::iow::io::make("OK") );
      else
        handler( ::iow::io::make("FAIL") );
    } );
  }

  void range_( std::shared_ptr<iprefixdb> db, std::stringstream& ss, wfc::iinterface::output_handler_t handler)
  {
    auto req = std::make_unique<request::range>();
    ss >> req->prefix;
    if (ss) ss >> req->from;
    if (ss) ss >> req->to;
    if (ss) ss >> req->offset;
    if (ss) ss >> req->limit;
    if ( req->limit == 0 )
      req->limit = 100;

    db->range( std::move(req), [handler](response::range::ptr res)
    {
      std::stringstream oss;
      if ( res!=nullptr ) for (const auto& field: res->fields )
      {
        oss << field.first << "=" << field.second << std::endl;
      }
      oss << res->status;
      handler( ::iow::io::make(oss.str()) );
    } );
  }

  void show_stat_( std::stringstream& oss, const wrtstat::aggregated_info& ag)
  {
        oss << "\tmin = " << ag.min << std::endl;
        oss << "\tmax = " << ag.max << std::endl;
        oss << "\tperc50 = " << ag.perc50 << std::endl;
        oss << "\tperc80 = " << ag.perc80 << std::endl;
        oss << "\tperc95 = " << ag.perc95 << std::endl;
        oss << "\tperc99 = " << ag.perc99 << std::endl;
        oss << "\tperc100 = " << ag.perc100 << std::endl;
        oss << "\tпогрешность = " << ag.lossy << std::endl;
  }

  void get_prefix_info_( std::shared_ptr<iprefixdb> db, std::stringstream& ss, wfc::iinterface::output_handler_t handler)
  {
    auto req = std::make_unique<request::range>();
    req->limit = 0;
    req->stat = true;
    req->nores = true;
    ss >> req->prefix;
    if (ss) ss >> req->limit;

    if ( req->prefix.empty() )
    {
      handler( iow::io::make("Error: Prefix required") );
      return;
    }

    db->range( std::move(req), [handler](response::range::ptr res)
    {
      std::stringstream oss;
      if (auto s = res->stat )
      {
        oss << "Ключей " << s->keys.count << std::endl;
        oss << "Значений " << s->values.count << std::endl;
        oss << "JSON-типы значений полей (количество):" << std::endl;
        oss << "\tJSON null = " << s->null_count << std::endl;
        oss << "\tJSON boolean = " << s->bool_count << std::endl;
        oss << "\tJSON number = " << s->number_count << std::endl;
        oss << "\tJSON string = " << s->string_count << std::endl;
        oss << "\tJSON array = " << s->array_count << std::endl;
        oss << "\tJSON object = " << s->object_count << std::endl;
        oss << "\trequired JSON repair = " << s->repair_count << std::endl;
        oss << "\tempty fields = " << s->empty_count << std::endl;
        oss << "Статистика по размерам ключей в байтах:" << std::endl;
        show_stat_(oss, s->keys);
        oss << "Статистика по размерам значений в байтах:" << std::endl;
        show_stat_(oss, s->values);

      }
      else
      {
        oss << "No Stat" << std::endl;
      }
      oss << res->status;
      handler( ::iow::io::make(oss.str()) );
    } );
  }

  void repair_json_( std::shared_ptr<iprefixdb> db, std::stringstream& ss, wfc::iinterface::output_handler_t handler)
  {
    auto req = std::make_unique<request::repair_json>();
    if (ss) ss >> req->prefix;
    if (ss) ss >> req->limit;

    if ( req->prefix == "*" )
      req->prefix.clear();
    db->repair_json( std::move(req), [handler](response::repair_json::ptr res)
    {
      if ( res->status == common_status::OK )
      {
        std::stringstream oss;
        oss << "prefix: " << res->prefix << std::endl;
        oss << "total: " << res->total << std::endl;
        oss << "repair: " << res->repaired << std::endl;
        oss << res->status;
        handler( ::iow::io::make(oss.str()) );
      }
      else
        handler( ::iow::io::make("FAIL") );
    } );
  }


  const char* help_str[][4] = {
    {"h", "help", "[<<command>>]",  "Подсказка по конкретной команде. Если не указана, то список всех комманд."},
    {"e", "exit", "",  "Выход."},
    {"db", "delay_background", "[0(delay seconds) [0(force) [<<prefix-list>>]]]", 
                               "Завершает все фоновые процессы на delay_s секунд/\n"
                               "Если force=1 то по завершению таймаута гарантировано запустит все фоновые \n"
                               "процессы, даже если в это время были вызовы delay_background c большим таймаутом. "
                               "Для гаранитированного запуска используй: 'db 0 1'"},
    {"cb", "continue_background", " [0(force) [<<prefix-list>>]]", ""},
    {"cp", "compact_prefix", "<<prefix>> [<<from>> [<<to>>] ]", "compact для префикса"},
    {"dp", "detach_prefixes",  "[0](access denied in sec) <<prefix1>> [<<prefix2>> ...]", "Отсоединяет префиксы на заданный таймаут. База префиксов перемещаеться в указанное \n"
                               "в конфигурации место. Префикс станет доступен через заданное первым параметром число секунд." },
    {"ap", "attach_prefixes",  "[0](open db) <<prefix1>> [<<prefix2>> ...]", "Присоединяет отсоединенные префиксы. База предварительно нужно переместить из директории куда префиксы были отсоеденены в дирректорию рабочих префиксов." },
    {"gap", "get_all_prefixes",  "", "Получить спискок всех доступных префиксов" },
    {"g", "get", "<<prefix>> <<key1>> [<<key2>> ....]", "Получить значения полей в указанном префиксе"},
    {"d", "del", "<<prefix>> <<key1>> [<<key2>> ....]", "Удалить поля в указанном префиксе"},
    {"s", "set", "<<prefix>> <<key>> <<value>>", "Изменить значение поля в указанном префиксе"},
    {"corrupt", "corrupt", "<<prefix>> <<key>> <<value>>", "Записать не-JSON значение повредив БД"},
    {"i", "inc", "<<prefix>> <<key>> <<increment>> [<<default value>>]", "Инкрементировать значение поля для указанном префиксе"},
    {"r", "range", "<<prefix>> [from [to [offset [limit] ] ]]  ", "Получить значения полей в указанном префиксе по диапазону. Примеры:\n"
      "\tr test - получить первые 25 полей \n"
      "\tr test key1 - получить первые 25 полей начиная с key1 \n"
      "\tr test key1 key2- получить первые 25 полей начиная с key1 до key2 включительно\n"
      "\tr test key1 key2 25 75 - получить 75 полей начиная с key1 до key2 включительно, пропустив первые 25"
    },
    {"rj", "repair_json", "[<<prefix>>(*)  [limit] ]", "Восстановить JSON-формат значений полей"},
    {"gpi", "get_prefix_info", "<<prefix>>  [limit] ", "Статистика по полям префикса"}
  };
  void help_( std::shared_ptr<iprefixdb> /*db*/, std::stringstream& ss, wfc::iinterface::output_handler_t handler)
  {
    std::stringstream oss;
    std::string cmd;
    ss >> cmd;
    for (auto hlp: help_str)
    {
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

void prefixdb_cmd( std::shared_ptr<iprefixdb> db, ::wfc::iinterface::data_ptr d, wfc::iinterface::output_handler_t handler)
{
  std::stringstream ss;
  ss << std::string( d->begin(), d->end() );
  std::string method;
  ss >> method;

  if ( method == "help" || method == "h")
  {
    help_(db, ss, std::move(handler) );
  }
  else if ( method == "exit" || method == "e")
  {
    handler(nullptr);
  }
  else if ( method == "db" || method=="delay_background")
  {
    delay_background_(db, ss, std::move(handler) );
  }
  else if ( method == "cb" || method=="continue_background")
  {
    continue_background_(db, ss, std::move(handler) );
  }
  else if ( method == "dp" || method=="detach_prefixes")
  {
    detach_prefixes_(db, ss, std::move(handler) );
  }
  else if ( method == "ap" || method=="attach_prefixes")
  {
    attach_prefixes_(db, ss, std::move(handler) );
  }
  else if ( method == "gap" || method=="get_all_prefixes")
  {
    get_all_prefixes_(db, ss, std::move(handler) );
  }
  else if ( method == "g" || method=="get")
  {
    get_(db, ss, std::move(handler) );
  }
  else if ( method == "s" || method=="set")
  {
    set_(db, ss, std::move(handler) );
  }
  else if ( method == "i" || method=="inc")
  {
    inc_(db, ss, std::move(handler) );
  }
  else if ( method == "d" || method=="del")
  {
    del_(db, ss, std::move(handler) );
  }
  else if ( method == "r" || method=="range")
  {
    range_(db, ss, std::move(handler) );
  }
  else if ( method=="cp" || method=="compact_prefix")
  {
    compact_prefix_(db, ss, std::move(handler) );
  }
  else if ( method=="rj" || method=="repair_json")
  {
    repair_json_(db, ss, std::move(handler) );
  }
  else if ( method=="corrupt")
  {
    corrupt_(db, ss, std::move(handler) );
  }
  else if ( method=="gpi" || method=="get_prefix_info")
  {
    get_prefix_info_(db, ss, std::move(handler) );
  }
  else
  {
    handler( ::iow::io::make("Unknown method") );
  }

}

}}}
