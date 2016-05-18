#define BOOST_NO_CXX11_SCOPED_ENUMS

#include "multidb.hpp"
#include "ifactory.hpp"
#include "god.hpp"
#include "aux/multidb.hpp"
#include "aux/scan_dir.hpp"
#include "aux/copy_dir.hpp"
#include <wfc/logger.hpp>
#include <wfc/boost.hpp>
#include <prefixdb/logger.hpp>

namespace wamba{ namespace prefixdb {

multidb::multidb()
{
  _suspend = false;
}

void multidb::suspend(bool val)
{
  _suspend = val;
}

bool multidb::reconfigure(const multidb_config& opt, std::shared_ptr<ifactory> factory)
{
  this->stop(); // ??? 
  {
    std::lock_guard<std::mutex> lk(_mutex);
    _factory = factory;
    _opt = opt;
    
    if ( _flow == nullptr )  
    {
      _flow = ::wfc::workflow::create(opt.workflow);
      _flow->start();
    }
    else 
    { 
      _flow->reconfigure( opt.workflow ); 
    }
    
    _opt.workflow_ptr = _flow;
    _opt.slave.timer = _flow;
    _factory->initialize(_opt/*, false*/);
    this->configure_backup_timer_();
    this->configure_archive_timer_();
    this->configure_prefix_reqester_();
  }
    
  if ( !::boost::filesystem::exists(opt.path) )
  {
    ::boost::system::error_code ec;
    ::boost::filesystem::create_directory(opt.path, ec);
    if (ec)
    {
      COMMON_LOG_ERROR("Create directory fail '" << opt.path << "'" << ec.message() );
      return false;
    }
  }
  
  return !opt.preopen || this->preopen_(opt.path, false);
}


void multidb::set( request::set::ptr req, response::set::handler cb)
{
  if ( suspended_<response::set>(req, cb) ) return;
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


void multidb::setnx( request::setnx::ptr req, response::setnx::handler cb)
{
  if ( suspended_<response::setnx>(req, cb) ) return;
  if ( !check_fields_<response::setnx>(req, cb) ) return;

  if ( auto db = this->prefix_(req->prefix, true) )
  {
    db->setnx( std::move(req), std::move(cb) );
  } 
  else 
  {
    create_prefix_fail<response::setnx>( std::move(req), std::move(cb) );
  }
}

void multidb::get( request::get::ptr req, response::get::handler cb)
{
  if ( suspended_<response::get>(req, cb) ) return;
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
  if ( suspended_<response::has>(req, cb) ) return;
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
  if ( suspended_<response::del>(req, cb) ) return;
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
  if ( suspended_<response::inc>(req, cb) ) return;
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
  if ( suspended_<response::add>(req, cb) ) return;
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
  if ( suspended_<response::packed>(req, cb) ) return;
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
  if ( suspended_<response::range>(req, cb) ) return;
  if ( notify_ban(req, cb) ) return;
  
  if ( req->limit + req->offset > this->_opt.range_limit )
  {
    send_error<common_status::RangeLimitExceeded, response::range>(std::move(req), std::move(cb) );
  }

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
  if ( suspended_<response::get_updates_since>(req, cb) ) return;
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

void multidb::get_all_prefixes( request::get_all_prefixes::ptr req, response::get_all_prefixes::handler cb)
{
  if ( suspended_<response::get_all_prefixes>(req, cb) ) return;
  if ( notify_ban(req, cb) ) return;
  
  auto res = std::make_unique<response::get_all_prefixes>();
  res->prefixes = this->all_prefixes_();
  cb(std::move(res));
}

void multidb::attach_prefixes( request::attach_prefixes::ptr req, response::attach_prefixes::handler cb)
{
  if ( suspended_<response::attach_prefixes>(req, cb) ) return;
  if ( req_null(req, cb) ) return;
  
  for (const auto& prefix : req->prefixes )
  {
    std::lock_guard<std::mutex> lk(this->_mutex);
    auto itr = this->_db_map.find(prefix);
    if ( itr != this->_db_map.end() && itr->second == nullptr )
    {
      _db_map.erase(itr);
      if ( req->opendb )
        this->prefix_(prefix, false);
    }
  }
}

void multidb::detach_prefixes( request::detach_prefixes::ptr req, response::detach_prefixes::handler cb)
{
  if ( suspended_<response::detach_prefixes>(req, cb) ) return;
  if ( req_null(req, cb) ) return;
  
  for (const auto& prefix : req->prefixes )
  {
    if ( auto db = this->prefix_(prefix, false) )
    {
      auto req1 = std::make_unique<request::detach_prefixes>();
      req1->prefixes.push_back(prefix);
      req1->deny_timeout_s = req->deny_timeout_s;
      db->detach_prefixes( std::move(req1), [db](response::detach_prefixes::ptr){/*захватываем db, на случай если detach_prefixes работает асинхронно*/});
      
      std::lock_guard<std::mutex> lk(_mutex);
      if ( req->deny_timeout_s == 0)
      {
        _db_map.erase( prefix );
      }
      else
      {
        _db_map[prefix]=nullptr;
        _flow->create_timer( 
          std::chrono::seconds(req->deny_timeout_s), 
          [this,prefix]()->bool 
          { 
            std::lock_guard<std::mutex> lk(this->_mutex);
            auto itr = this->_db_map.find(prefix);
            if ( itr != this->_db_map.end() && itr->second == nullptr )
            {
              _db_map.erase(itr);
            }
            return false;
          }
        );
      }
    }
  }
  
  if ( cb!= nullptr )
  {
    auto res = std::make_unique<response::detach_prefixes>();
    cb( std::move(res) );
  }
}

void multidb::delay_background( request::delay_background::ptr req, response::delay_background::handler cb)
{
  if ( suspended_<response::delay_background>(req, cb) ) return;
  if ( req_null(req, cb) ) return;
  
  auto prefixes = std::move(req->prefixes);

  if ( prefixes.empty() )
    prefixes = this->all_prefixes_();

  for ( const std::string& prefix: prefixes)
  {
    if ( auto db = this->prefix_(prefix, false) )
    {
      db->delay_background( std::make_unique<request::delay_background>(*req), nullptr );
    }
  }
  
  if ( cb != nullptr )
    cb(std::make_unique<response::delay_background>());
}


void multidb::continue_background( request::continue_background::ptr req, response::continue_background::handler cb)
{
  if ( suspended_<response::continue_background>(req, cb) ) return;
  if ( req_null(req, cb) ) return;
  
  auto prefixes = std::move(req->prefixes);

  if ( prefixes.empty() )
    prefixes = this->all_prefixes_();

  for ( const std::string& prefix: prefixes)
  {
    if ( auto db = this->prefix_(prefix, false) )
    {
      db->continue_background( std::make_unique<request::continue_background>(*req), nullptr );
    }
  }
  
  if ( cb != nullptr )
    cb(std::make_unique<response::continue_background>());
}



void multidb::stop()
{
  if ( _flow )
    _flow->stop();

  std::lock_guard<std::mutex> lk(_mutex);
  _flow = nullptr;
  if ( _factory )
  {
    CONFIG_LOG_BEGIN("STOP DB...")
    _factory = nullptr;
    for (auto& db : _db_map)
    {
      if ( db.second!= nullptr )
        db.second->stop();
      db.second=nullptr;
    }
    _db_map.clear();
    CONFIG_LOG_END("STOP DB!")
  }
}


void multidb::start() 
{
}


bool multidb::backup()
{
  if ( !::boost::filesystem::exists(_opt.backup.path) )
  {
    ::boost::system::error_code ec;
    ::boost::filesystem::create_directory(_opt.backup.path, ec);
    if (ec)
    {
      COMMON_LOG_ERROR("Create directory fail '" << _opt.backup.path << "'" << ec.message() );
      return true;
    }
  }

  auto prefixes = this->all_prefixes_();
  size_t count = 0;
  for ( const std::string& prefix: prefixes)
  {
    if ( auto db = this->prefix_(prefix, false) )
    {
      bool result = db->backup();
      if ( !result )
      {
        // При неудачном бэкапе дериктория перемещаеться 
        this->close_prefix_( prefix );
        db = this->prefix_(prefix, false);
        if ( db != nullptr )
        {
          // При повторном открытии создаеться полный бэкап (на пустой директории)
          result = db->backup();
        }
      }
      count += result;
    }
  }
  return count == prefixes.size();
}


std::string time_string()
{
  std::tm ti;
  std::time_t now = time(0);
  localtime_r( &now, &ti );
  char buf[80]={0};
  strftime( buf, 80, "%Y%m%d-%H%M%S", &ti);
  return std::string(buf);
}


bool multidb::restore()
{
  if ( !::boost::filesystem::is_directory(this->_opt.restore.path) )
  {
    DOMAIN_LOG_ERROR( "Restore FAIL: '" << this->_opt.restore.path << "' is not directory" )
    return false;
  }
  
  if ( ::boost::filesystem::exists(this->_opt.path) )
  {
    ::boost::system::error_code ec;
    std::string bakpath = this->_opt.path + "_" + time_string();
    ::boost::filesystem::rename(this->_opt.path, bakpath, ec );
    if ( ec )
    {
      DOMAIN_LOG_ERROR( "Rename old storage FAIL: '" << this->_opt.restore.path << "' -> '" << bakpath << "'" )
      return false;
    }
    
    ec.clear();
    ::boost::filesystem::create_directory(this->_opt.path, ec);
    if ( ec )
    {
      DOMAIN_LOG_ERROR( "Create directory FAIL: '" << this->_opt.path << "'" )
      return false;
    }
  }
  
  bool fail = false;
  auto prefixes = scan_dir(_opt.restore.path, fail);
  DOMAIN_LOG_MESSAGE("Префиксов найдено: " << prefixes.size() << " в " << _opt.restore.path) 
  if (fail) return false;
  
  bool result = true;
  for ( const std::string& prefix: prefixes)
  {
    DOMAIN_LOG_MESSAGE("Востановление для " << prefix) 
    if ( auto rocks_resor =  _factory->create_restore(prefix) )
    {
      result &= rocks_resor->restore();
    }
  }
  return result;
}


bool multidb::archive()
{
  if ( !_opt.archive.enabled )
    return false;
  
  auto path = _opt.archive.path;
  if ( !::boost::filesystem::exists(path) )
  {
    ::boost::system::error_code ec;
    ::boost::filesystem::create_directory(path, ec);
    if (ec)
    {
      COMMON_LOG_ERROR("Create directory fail '" << path << "'" << ec.message() );
      return false;
    }
  }
  
  bool fail = false;
  auto dirs = scan_dir(path, fail);
  if ( _opt.archive.depth <= dirs.size() )
  {
    std::sort(dirs.begin(), dirs.end() );
    std::for_each( dirs.begin(), dirs.begin() + dirs.size() - _opt.archive.depth, [path](const std::string& name)
    {
      auto dir = path + '/' + name;
      std::string message;
      if ( !delete_dir(dir, message) )
      {
        PREFIXDB_LOG_ERROR("Delete old archive '" << path << "': " << message)
      }
    });
  }

  path += "/" + time_string();
  ::boost::system::error_code ec;
  if( !::boost::filesystem::create_directory( path, ec) )
  {
    COMMON_LOG_ERROR("Create dir " << path << " FAIL: " << ec.message() );
    return false;
  }
  
  auto prefixes = this->all_prefixes_();
  bool result = true;
  for ( const std::string& prefix: prefixes)
  {
    if ( auto db = this->prefix_(prefix, false) )
    {
      result &= db->archive(path);
    }
  }
  return result;
}



//
// ---------------------------
//


void multidb::configure_archive_timer_()
{
  if ( _archive_timer != -1 ) _flow->release_timer(_archive_timer);
  if ( _opt.archive.enabled && !_opt.archive.path.empty() )
  {
    std::weak_ptr<multidb> wthis = this->shared_from_this();
    _archive_timer = _flow->create_timer(
      _opt.archive.start_time,
      std::chrono::seconds( _opt.archive.period_s ),
      [this]()->bool { this->archive(); return true; }
    );
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


void multidb::configure_backup_timer_()
{
  if ( _backup_timer != -1 ) _flow->release_timer(_backup_timer);
  
  if ( _opt.backup.enabled &&  !_opt.backup.path.empty() )
  {
    std::weak_ptr<multidb> wthis = this->shared_from_this();
    _backup_timer = _flow->create_timer(
      _opt.backup.start_time,
      std::chrono::seconds( _opt.backup.period_s ),
      [this]() { this->backup(); return true; }
    );
  }
}


void multidb::configure_prefix_reqester_()
{
  if ( _prefix_reqester != -1 ) _flow->release_timer(_prefix_reqester);
  
  if ( !_opt.slave.enabled ) return;
  
  std::weak_ptr<multidb> wthis = this->shared_from_this();
  _prefix_reqester = _flow->create_requester<request::get_all_prefixes, response::get_all_prefixes>
  (
    std::chrono::milliseconds( _opt.slave.query_prefixes_timeout_ms ),
    this->_opt.slave.master,
    &iprefixdb::get_all_prefixes,
    std::bind( &multidb::get_all_prefixes_handler_, this, std::placeholders::_1)
  );
}


request::get_all_prefixes::ptr multidb::get_all_prefixes_handler_(response::get_all_prefixes::ptr res)
{
  if ( res == nullptr )
    return std::make_unique<request::get_all_prefixes>();

  auto preflist = this->all_prefixes_();
  std::set<std::string> prefset( preflist.begin(), preflist.end() );
  if ( res->status == common_status::OK)
  {
    for ( const auto& x : res->prefixes )
    {
      if ( x.empty() )
      {
        PREFIXDB_LOG_WARNING("get_all_prefixes: empty prefix")
        continue;
      }
      this->prefix_(x, true);
      prefset.erase(x);
    }
        
    if ( !prefset.empty() )
    {
      auto preq = std::make_shared<request::detach_prefixes>();
      preq->deny_timeout_s = 0;
      preq->prefixes.reserve(prefset.size());
      for ( auto& x : prefset )
      {
        preq->prefixes.push_back( std::move(x) );
      }

      _flow->post([this, preq]
      {
        this->detach_prefixes( std::make_unique<request::detach_prefixes>(*preq), nullptr );
      });
    }
  }
  else if ( res != nullptr )
  {
    PREFIXDB_LOG_WARNING( "multidb::configure_prefix_reqester_ result get_all_prefixes not valid status " << int(res->status) );
  }
  return nullptr;
}


std::vector< std::string > multidb::all_prefixes_()
{
  std::lock_guard<std::mutex> lk(_mutex);
  
  std::vector< std::string > result;
  result.reserve( _db_map.size() );
  for (const auto& p : _db_map)
  {
    if ( p.second != nullptr && !p.first.empty() )
    {
      result.push_back(p.first);
    }
  }
  return std::move(result);
}


bool multidb::close_prefix_(const std::string& prefix)
{
  std::lock_guard<std::mutex> lk(_mutex);
  auto itr = _db_map.find(prefix);
  if ( itr == _db_map.end())
    return false;
  if ( itr->second != nullptr )
  {
    itr->second->stop();
    itr->second = nullptr;
  }
  _db_map.erase(itr);
  return true;
}

/// Если create_if_missing всегда возвращает объект,
/// в противном случае, только если база существует 
multidb::prefixdb_ptr multidb::prefix_(const std::string& prefix, bool create_if_missing)
{
  if ( prefix.empty() )
    return nullptr;
  
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
  
  if ( !create_if_missing )
  {
    // Сначала проверяем, что директория существует
    // чтобы rockdb не оставляла мусор, при открытии без флага create_if_missing
    auto path = _opt.path + "/" + prefix;
    if ( !::boost::filesystem::exists(path) )
      return nullptr;
  }
  
  if ( auto db = _factory->create_db(prefix, create_if_missing) )
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

template<typename Res, typename ReqPtr, typename Callback>
bool multidb::suspended_(const ReqPtr& /*req*/, const Callback& cb)
{
  if ( !this->_suspend ) return false;
  if ( cb != nullptr )
  {
    cb( std::make_unique<Res>() );
  }
  return true;
}

template<typename Res, typename ReqPtr, typename Callback>
bool multidb::check_prefix_(const ReqPtr& req, const Callback& cb)
{
  if ( req->prefix.empty() )
    return send_error<common_status::EmptyPrefix, Res>(std::move(req), std::move(cb) );
  
  if ( _opt.prefix_size_limit!=0 && req->prefix.size() > _opt.prefix_size_limit )
    return send_error<common_status::PrefixLengthExceeded, Res>(std::move(req), std::move(cb) );
  
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
    return send_error<common_status::TooManyKeys, Res>(std::move(req), std::move(cb) );
  
  if ( _opt.value_size_limit==0 || _opt.key_size_limit==0 )
    return true;
  
  for (const auto& f : req->fields)
  {
    if ( f.first.size() > _opt.key_size_limit)
      return send_error<common_status::KeyLengthExceeded, Res>(std::move(req), std::move(cb) );
      
    if ( f.second.size() > _opt.value_size_limit)
      return send_error<common_status::ValueLengthExceeded, Res>(std::move(req), std::move(cb) );
  }
  return true;
}



}}
