#include <wfc/logger.hpp>
#include "multidb.hpp"
#include "ifactory.hpp"
#include "god.hpp"
#include "aux/multidb.hpp"
#include "aux/scan_dir.hpp"

#include <boost/filesystem.hpp>

namespace wamba{ namespace prefixdb {

void multidb::close()
{
  std::lock_guard<std::mutex> lk(_mutex);
  if ( _factory )
  {
    CONFIG_LOG_BEGIN("STOP DB...")
    _factory = nullptr;
    for (auto& db : _db_map)
    {
      if ( db.second!= nullptr )
        db.second->close();
      db.second=nullptr;
    }
    _db_map.clear();
    CONFIG_LOG_END("STOP DB!")
  }
}

bool multidb::preopen_(std::string path, bool create_if_missing)
{
  bool fail = false;
  auto dirs = scan_dir(path, fail);
  if (fail)
  {
    CONFIG_LOG_FATAL("Directory " << path << " is missing");
    return false;
  }
  
  CONFIG_LOG_BEGIN("Pre open prefixes ...")
  for (auto name: dirs)
  {
    CONFIG_LOG_MESSAGE("Pre open prefix " << name << "...")
    if ( nullptr == this->prefix_(name, create_if_missing) )
    {
      CONFIG_LOG_WARNING("Pre open prefix FAIL")
    }
  }
  CONFIG_LOG_END("Pre open prefixes")
  return true;
}

void multidb::start() 
{
  // not used
}

bool multidb::reconfigure(const multidb_config& opt, std::shared_ptr<ifactory> factory)
{
  this->close();
  {
    std::lock_guard<std::mutex> lk(_mutex);
    _factory = factory;
    _opt = opt;
    /*CONFIG_LOG_MESSAGE("CREATE FACTORY...")
    _factory = god::create("rocksdb", this->global()->);
    _factory->initialize(opt);
    */
    //_factory->initialize(opt.path, opt.backup_path, opt.restore_path, opt.ini);
  }
  
  return !opt.preopen || this->preopen_(opt.path, false);
  /*
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
      if ( nullptr == this->prefix_(name, false) )
      {
        CONFIG_LOG_WARNING("Pre open prefix FAIL")
      }
    }
    CONFIG_LOG_END("Pre open prefixes")
  }
  */
  return true;
}

template<typename Res, typename ReqPtr, typename Callback>
bool multidb::check_prefix_(const ReqPtr& req, const Callback& cb)
{
  if ( req->prefix.empty() )
  {
    send_error<common_status::EmptyPrefix, Res>(std::move(req), std::move(cb) );
    return false;
  }
  
  if ( _opt.prefix_size_limit!=0 && req->prefix.size() > _opt.prefix_size_limit )
  {
    send_error<common_status::PrefixLengthExceeded, Res>(std::move(req), std::move(cb) );
    return false;
  }
  
  return true;
}


template<typename Res, typename ReqPtr, typename Callback>
bool multidb::check_fields_(const ReqPtr& req, const Callback& cb)
{
  if ( empty_fields<Res>(req, cb) )
    return false;

  if ( !this->check_prefix_<Res>(req, cb) )
    return false;
  
  if ( _opt.keys_per_req!=0 && req->fields.size() > _opt.keys_per_req )
  {
    send_error<common_status::TooManyKeys, Res>(std::move(req), std::move(cb) );
    return false;
  }
  
  if ( _opt.value_size_limit==0 || _opt.key_size_limit==0 )
    return true;
  
  for (const auto& f : req->fields)
  {
    if ( f.first.size() > _opt.key_size_limit)
    {
      send_error<common_status::KeyLengthExceeded, Res>(std::move(req), std::move(cb) );
      return false;
    }

    if ( f.second.size() > _opt.value_size_limit)
    {
      send_error<common_status::ValueLengthExceeded, Res>(std::move(req), std::move(cb) );
      return false;
    }
  }
  return true;
}


/// Если create_if_missing всегда возвращает объект,
/// в противном случае, только если база существует 
multidb::prefixdb_ptr multidb::prefix_(const std::string& prefix, bool create_if_missing)
{
  std::lock_guard<std::mutex> lk(_mutex);
  
  if ( _factory == nullptr )
  {
    DOMAIN_LOG_ERROR("multidb не сконфигурирован!")
    return nullptr;
  }
  
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
  
  if ( _opt.max_prefixes!=0 && _db_map.size() >= _opt.max_prefixes )
    return nullptr;
  
  if ( auto db = _factory->create(prefix, create_if_missing) )
  {
    COMMON_LOG_MESSAGE("Открыт новый префикс: " << prefix)
    _db_map.insert(itr, std::make_pair(prefix, db));
    db->start();
    return db;
  }
  
  // Запоминаем, чтобы не создавать заного
  _db_map[prefix] = nullptr;
  return nullptr;
}

std::vector< std::string > multidb::all_prefixes_()
{
  std::vector< std::string > result;
  result.reserve( _db_map.size() );
  for (const auto& p : _db_map)
    result.push_back(p.first);
  return std::move(result);
}

void multidb::set( request::set::ptr req, response::set::handler cb)
{
  if ( !check_fields_<response::set>(req, cb) ) return;

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
  if ( empty_fields<response::del>(req, cb) ) 
    return;

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
  if ( !check_fields_<response::inc>(req, cb) ) return;

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
  if ( !check_fields_<response::add>(req, cb) ) return;

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
  if ( !check_fields_<response::packed>(req, cb) ) return;

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

void multidb::get_updates_since( request::get_updates_since::ptr req, response::get_updates_since::handler cb)
{
  if ( notify_ban(req, cb) ) return;

  if ( auto db = this->prefix_(req->prefix, false) )
  {
    db->get_updates_since( std::move(req), std::move(cb) );
  }
  else
  {
    prefix_not_found<response::get_updates_since>( std::move(req), std::move(cb) );
  }  
}


void multidb::backup(bool compact_range)
{
  auto prefixes = this->all_prefixes_();
  
  for ( const std::string& prefix: prefixes)
  {
    DEBUG_LOG_MESSAGE("Backup for: " << prefix << "..." )
    if ( auto db = this->prefix_(prefix, false) )
    {
      db->backup( compact_range );
    }
  }
}

void multidb::backup( request::backup::ptr req, response::backup::handler cb) 
{
  
  // ТОDO: сделать отдельную ветку для ручного бэкапа
  auto prefixes = std::move(req->prefixes);
  if ( prefixes.empty() )
    prefixes = this->all_prefixes_();
  
  for ( const std::string& prefix: prefixes)
  {
    DEBUG_LOG_MESSAGE("Backup for: " << prefix << "..." )
    if ( auto db = this->prefix_(prefix, false) )
    {
      db->backup( 
        std::make_unique<request::backup>(*req),
        nullptr
      );
    }
    else
    {
        // TODO: внести в массив статусов, если sync
    }
  }
  
  if ( cb != nullptr )
  {
    auto res = std::make_unique<response::backup>();
    cb( std::move(res) );
  }
  
 }

void multidb::archive(std::string suffix)
{
  std::tm ti;
  std::time_t now = time(0);
  localtime_r( &now, &ti );
  char buf[80]={0};
  strftime( buf, 80, "/%Y%m%d-%H%M%S/", &ti);
  suffix += buf;
  
  ::boost::system::error_code ec;
  if( !::boost::filesystem::create_directory( _opt.archive_path + suffix, ec) )
  {
    COMMON_LOG_ERROR("Create dir " << _opt.archive_path + suffix << " FAIL: " << ec.message() );
    return;
  }
  
  auto prefixes = this->all_prefixes_();
  
  for ( const std::string& prefix: prefixes)
  {
    DEBUG_LOG_MESSAGE("Archive for: " << prefix << "... " )
    
    if ( auto db = this->prefix_(prefix, false) )
    {
      db->archive(suffix);
    }
  }
}

void multidb::restore()
{
  this->preopen_(this->_opt.restore_path, true);
  
  auto prefixes = this->all_prefixes_();
  
  for ( const std::string& prefix: prefixes)
  {
    DEBUG_LOG_MESSAGE("Restore for: " << prefix << "..." )
    if ( auto db = this->prefix_(prefix, false) )
    {
      db->restore( );
    }
  }

  /*
  auto req = std::make_unique<request::backup>();
  this->backup(std::move(req), nullptr);
  */
}

void multidb::restore( request::restore::ptr /*req*/, response::restore::handler /*cb*/) 
{
}

}}
