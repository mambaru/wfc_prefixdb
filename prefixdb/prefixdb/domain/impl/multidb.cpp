#include "multidb.hpp"
#include "ifactory.hpp"
#include "god.hpp"
#include <wfc/logger.hpp>

namespace wamba{ namespace prefixdb {
  
namespace
{
  
    template<typename R, typename Callback>
    inline void prefix_not_found(Callback cb)
    {
      if ( cb!=nullptr )
      {
        std::unique_ptr<R> res;
        res->status = common_status::PrefixNotFound;
        cb( std::move(res) );
      }
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
          res->status = common_status::OK;
          cb( std::move(res) );
        }
        return true;
      }
      return false;
    }
}
  
void multidb::stop()
{
  _factory = nullptr;
}

void multidb::reconfigure(const multidb_options opt)
{
  std::lock_guard<std::mutex> lk(_mutex);
  _factory = god::create(opt.type);
  _factory->initialize(opt.path, opt.ini);
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
    prefix_not_found<response::get>( std::move(cb) );
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
    prefix_not_found<response::has>( std::move(cb) );
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
    prefix_not_found<response::del>( std::move(cb) );
  }
}

void multidb::inc( request::inc::ptr req, response::inc::handler cb) 
{
  if ( empty_fields<response::inc>(req, cb) ) return;

  if ( auto db = this->prefix_(req->prefix, true) )
  {
    db->inc( std::move(req), std::move(cb) );
  }
}

void multidb::upd( request::upd::ptr req, response::upd::handler cb) 
{
  if ( empty_fields<response::upd>(req, cb) ) return;

  if ( auto db = this->prefix_(req->prefix, true) )
  {
    db->upd( std::move(req), std::move(cb) );
  }
}

}}
