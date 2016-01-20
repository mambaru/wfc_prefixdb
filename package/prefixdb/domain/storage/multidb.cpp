#include "multidb.hpp"
#include "ifactory.hpp"
#include "god.hpp"
#include <wfc/logger.hpp>


#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

namespace wamba{ namespace prefixdb {
 
/*
 * ЖШЖ
 * iii
 * ...
 */
namespace
{
    template<common_status Status, typename Res, typename ReqPtr, typename Callback>
    inline void send_error(const ReqPtr& req, const Callback& cb)
    {
      if ( cb!=nullptr )
      {
        auto res = std::make_unique<Res>();
        res->prefix = std::move(req->prefix);
        res->status = Status;
        cb( std::move(res) );
      }
    }

    template<typename Res, typename ReqPtr, typename Callback>
    inline void prefix_not_found(const ReqPtr& req, const Callback& cb)
    {
      send_error<common_status::PrefixNotFound, Res>(std::move(req), std::move(cb) );
    }

    template<typename Res, typename ReqPtr, typename Callback>
    inline void create_prefix_fail(const ReqPtr& req, const Callback& cb)
    {
      send_error<common_status::CreatePrefixFail, Res>(std::move(req), std::move(cb) );
    }

    template<typename Req, typename Callback>
    inline bool req_null(const Req& req, const Callback& cb)
    {
      if ( req==nullptr )
      {
        if ( cb!=nullptr )
        {
          cb(nullptr);
        }
        return true;
      }
      return false;
    }

    template<typename Req, typename Callback>
    inline bool notify_ban(const Req& req, const Callback& cb)
    {
      return cb==nullptr || req_null(req, cb);
    }

    template<typename Res, typename ReqPtr, typename Callback>
    inline bool empty_fields(const ReqPtr& req, const Callback& cb)
    {
      if ( req_null(req, cb) ) return true; 

      if ( req->fields.empty() )
      {
        if ( cb != nullptr )
        {
          auto res = std::make_unique<Res>();
          res->prefix = std::move(req->prefix);
          res->status = common_status::EmptyFields;
          cb( std::move(res) );
        }
        return true;
      }
      return false;
    }
    
    inline std::vector<std::string> scan_dir(std::string path, bool& fail)
    {
      fail = false;
      std::vector<std::string> result;

      DIR *dir;
      struct dirent *entry;

      dir = opendir(path.c_str());
      if (!dir) 
      {
        // perror("diropen");
        fail = true;
        return result;
      };

      while ( (entry = readdir(dir)) != NULL) 
      {
        if ( entry->d_type == 4)
        {
          std::string file = entry->d_name;
          if ( file=="." || file==".." ) continue;
          result.push_back(file);
        }
      };

      closedir(dir);
      return result;
    }
}


  
void multidb::stop()
{
  _factory = nullptr;
}

bool multidb::reconfigure(const multidb_options opt)
{
  {
    std::lock_guard<std::mutex> lk(_mutex);
    CONFIG_LOG_MESSAGE("CREATE FACTORY...")
    _factory = god::create(opt.type);
    _factory->initialize(opt.path, opt.ini);
  }
  
  bool fail = false;
  auto dirs = scan_dir(opt.path, fail);
  if (fail)
  {
    CONFIG_LOG_FATAL("Directory " << opt.path << " is missing");
    return false;
  }
  
  if ( opt.preopen )
  {
    CONFIG_LOG_BEGIN("Pre open prefixes ...")
    for (auto name: dirs)
    {
      CONFIG_LOG_MESSAGE("Pre open prefix " << name << "...")
      if (auto db = this->prefix_(name, false) )
      {
        // CONFIG_LOG_MESSAGE("Pre open prefix OK")
      }
      else
      {
        CONFIG_LOG_WARNING("Pre open prefix FAIL")
      }
      //CONFIG_LOG_END("Pre open prefix DONE")
    }
    CONFIG_LOG_END("Pre open prefixes")
  }
  return true;
}

/// Если create_if_missing всегда возвращает объект,
/// в противном случае, только если база существует 
multidb::prefixdb_ptr multidb::prefix_(const std::string& prefix, bool create_if_missing)
{
  std::lock_guard<std::mutex> lk(_mutex);
  prefixdb_ptr result = nullptr;
  
  auto itr = _db_map.find(prefix);
  if ( itr != _db_map.end() )
  {
    if ( itr->second)
      return itr->second;
    
    // Для get не создаем для несуществующих префиксов
    if ( !create_if_missing )
      return nullptr;
  }
  
  if ( auto db = _factory->create(prefix, create_if_missing) )
  {
    COMMON_LOG_MESSAGE("Открыт новый префикс: " << prefix)
    _db_map.insert(itr, std::make_pair(prefix, db));
    return db;
  }
  
  // Запоминаем, чтобы не создавать заного
  _db_map[prefix] = nullptr;
  return nullptr;
}

void multidb::set( request::set::ptr req, response::set::handler cb)
{
  if ( empty_fields<response::set>(req, cb) ) return;

  if ( auto db = this->prefix_(req->prefix, true) )
  {
    db->set( std::move(req), std::move(cb) );
  } 
  else 
  {
    create_prefix_fail<response::set>( std::move(req), std::move(cb) );
  }
}

void multidb::get( request::get::ptr req, response::get::handler cb)
{
  if ( notify_ban(req, cb) ) return;

  if ( auto db = this->prefix_(req->prefix, false) )
  {
    db->get( std::move(req), std::move(cb) );
  }
  else
  {
    prefix_not_found<response::get>( std::move(req), std::move(cb) );
  }
}

void multidb::has( request::has::ptr req, response::has::handler cb)
{
  if ( notify_ban(req, cb) ) return;

  if ( auto db = this->prefix_(req->prefix, false) )
  {
    db->has( std::move(req), std::move(cb) );
  }
  else
  {
    prefix_not_found<response::has>( std::move(req), std::move(cb) );
  }
}


void multidb::del( request::del::ptr req, response::del::handler cb) 
{
  if ( empty_fields<response::del>(req, cb) ) return;

  if ( auto db = this->prefix_(req->prefix, false) )
  {
    db->del( std::move(req), std::move(cb) );
  }
  else
  {
    prefix_not_found<response::del>( std::move(req), std::move(cb) );
  }
}

void multidb::inc( request::inc::ptr req, response::inc::handler cb) 
{
  if ( empty_fields<response::inc>(req, cb) ) return;

  if ( auto db = this->prefix_(req->prefix, true) )
  {
    db->inc( std::move(req), std::move(cb) );
  }
  else 
  {
    create_prefix_fail<response::inc>( std::move(req), std::move(cb) );
  }
}

void multidb::add( request::add::ptr req, response::add::handler cb) 
{
  if ( empty_fields<response::add>(req, cb) ) return;

  if ( auto db = this->prefix_(req->prefix, true) )
  {
    db->add( std::move(req), std::move(cb) );
  }
  else 
  {
    create_prefix_fail<response::add>( std::move(req), std::move(cb) );
  }
}

void multidb::packed( request::packed::ptr req, response::packed::handler cb)
{
  if ( empty_fields<response::packed>(req, cb) ) return;

  if ( auto db = this->prefix_(req->prefix, true) )
  {
    db->packed( std::move(req), std::move(cb) );
  }
  else 
  {
    create_prefix_fail<response::packed>( std::move(req), std::move(cb) );
  }
}

void multidb::range( request::range::ptr req, response::range::handler cb)
{
  if ( notify_ban(req, cb) ) return;

  if ( auto db = this->prefix_(req->prefix, false) )
  {
    db->range( std::move(req), std::move(cb) );
  }
  else
  {
    prefix_not_found<response::range>( std::move(req), std::move(cb) );
  }
}


}}
