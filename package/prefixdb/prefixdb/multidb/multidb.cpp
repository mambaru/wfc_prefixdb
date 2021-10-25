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
#include <algorithm>

namespace wamba{ namespace prefixdb {

namespace {
  std::string time_string()
  {
    std::tm ti;
    std::time_t now = time(nullptr);
    localtime_r( &now, &ti );
    char buf[80]={0};
    strftime( buf, 80, "%Y%m%d-%H%M%S", &ti);
    return std::string(buf);
  }
}

multidb::multidb():
  // реконфигурируемые опции
  _range_limit(),
  _max_prefixes(),
  _prefix_size_limit(),
  _keys_per_req(),
  _value_size_limit(),
  _key_size_limit()
{
}

bool multidb::configure(const multidb_config& opt, const std::shared_ptr<ifactory>& factory)
{
  {
    std::lock_guard<std::mutex> lk(_mutex);
    _factory = factory;
    _opt = opt;
    _workflow = opt.args.timers_workflow;
    _opt.slave.timer = _workflow;
    if ( !_factory->initialize(_opt) )
      return false;
  }

  if ( !::boost::filesystem::exists(opt.path) )
  {
    ::boost::system::error_code ec;
    ::boost::filesystem::create_directory(opt.path, ec);
    if (ec)
    {
      PREFIXDB_LOG_ERROR("Create directory fail (main)'" << opt.path << "'" << ec.message() );
      return false;
    }
  }
  
  _writable_prefixes = opt.writable_prefixes;
  _readonly_prefixes = opt.readonly_prefixes;
  std::sort(_writable_prefixes.begin(), _writable_prefixes.end());
  std::sort(_readonly_prefixes.begin(), _readonly_prefixes.end());

  _slave_writable_only = opt.slave.writable_only;
  _slave_allowed_prefixes = opt.slave.allowed_prefixes;
  _slave_denied_prefixes = opt.slave.denied_prefixes;
  std::sort(_slave_allowed_prefixes.begin(), _slave_allowed_prefixes.end());
  std::sort(_slave_denied_prefixes.begin(), _slave_denied_prefixes.end());

  // Еще базы не открыты, конфигурация только этого
  this->reconfigure(opt);

  if (!opt.preopen)
    return true;
  if ( !this->preopen_(opt.path, false) )
    return false;
  if (opt.compact.startup_compact)
    return this->compact();
  return true;
}

void multidb::reconfigure(const multidb_config& opt)
{
  std::lock_guard<std::mutex> lk(_mutex);
  _range_limit  = opt.range_limit;
  _max_prefixes = opt.max_prefixes;
  _prefix_size_limit = opt.prefix_size_limit;
  _keys_per_req = opt.keys_per_req;
  _value_size_limit = opt.value_size_limit;
  _key_size_limit = opt.key_size_limit;

  for (auto db_val: _db_map)
    if ( auto db = db_val.second)
      db->reconfigure(opt);
}


void multidb::start()
{
  std::lock_guard<std::mutex> lk(_mutex);
  this->configure_backup_timer_();
  this->configure_archive_timer_();
  this->configure_prefix_reqester_();
  this->configure_compact_timer_();
}

void multidb::set( request::set::ptr req, response::set::handler cb)
{
  if ( !check_fields_<response::set>(req, cb, true) )
    return;

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
  if ( !check_fields_<response::setnx>(req, cb, true) )
    return;

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

  if ( !is_writable_<response::del>(req, cb) )
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
  if ( !check_fields_<response::inc>(req, cb, true) )
    return;

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
  if ( !check_fields_<response::add>(req, cb, true) )
    return;

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
  if ( !check_fields_<response::packed>(req, cb, true) )
    return;

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
  if ( _range_limit!=0 && (req->limit + req->offset > _range_limit) )
  {
    send_error<common_status::RangeLimitExceeded, response::range>(std::move(req), std::move(cb) );
    return;
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

void multidb::repair_json( request::repair_json::ptr req, response::repair_json::handler cb)
{
  //пустой префикс - для всех преффиксов
  if ( req->prefix.empty() )
  {
    PREFIXDB_LOG_BEGIN("Repair JSON for ALL prefixes...")
    req->nores = true;
    req->noval = true;
    req->beg = true;
    req->from = "";
    req->to = "";
    req->offset = 0;
    // req->limit = 0; оставил для тестирования

    size_t total = 0;
    size_t repaired = 0;
    std::stringstream sprefixes;
    auto prefixes = this->all_prefixes_();
    for ( std::string prefix : prefixes )
    {
      if ( auto db = this->prefix_(prefix, false) )
      {
        auto prefix_req = std::make_unique<request::repair_json>(*req);
        prefix_req->prefix = prefix;
        db->repair_json(std::move(prefix_req), [&total, &repaired](response::repair_json::ptr res){
          total += res->total;
          repaired += res->repaired;
        });
        sprefixes << prefix << ",";
      }
    }
    if ( cb != nullptr )
    {
      auto res = std::make_unique<response::repair_json>();
      res->fin = true;
      res->total = total;
      res->repaired = repaired;
      res->prefix = sprefixes.str();
      if ( !res->prefix.empty() )
        res->prefix.pop_back();
      cb(std::move(res));
    }
    PREFIXDB_LOG_END("Repair JSON for ALL prefixes Done! repaired=" << repaired << " total=" << total << " prefixes=" << sprefixes.str() )
  }
  else if ( auto db = this->prefix_(req->prefix, false) )
  {
    db->repair_json( std::move(req), std::move(cb) );
  }
  else
  {
    prefix_not_found<response::repair_json>( std::move(req), std::move(cb) );
  }
}


void multidb::get_updates_since( request::get_updates_since::ptr req, response::get_updates_since::handler cb)
{
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
  auto res = std::make_unique<response::get_all_prefixes>();
  auto prefixes = this->all_prefixes_();
  res->prefixes.reserve( prefixes.size() );
  for ( const std::string& prefix: prefixes)
  {
    if ( !req->writable_only || this->is_writable_(prefix) )
      res->prefixes.push_back(prefix);
  }
  cb(std::move(res));
}

void multidb::attach_prefixes( request::attach_prefixes::ptr req, response::attach_prefixes::handler cb)
{
  bool ready = false;
  for (const auto& prefix : req->prefixes )
  {
    {
      std::lock_guard<std::mutex> lk(this->_mutex);
      auto path1 = _opt.path + "/" + prefix;
      auto path2 = _opt.detach_path + "/" + prefix;
      if ( file::exist(path2) && !file::exist(path1) )
      {
        std::string errmsg;
        if ( !file::move(path2, path1, errmsg) )
        {
          PREFIXDB_LOG_ERROR("Attach detached prefix '" << prefix << "': " << errmsg);
        }
        else
        {
          PREFIXDB_LOG_MESSAGE("Moved attached prefix '" << prefix << "' from " << path2 << " to " << path1 );
        }
      }
      auto itr = this->_db_map.find(prefix);
      if ( itr != this->_db_map.end() && itr->second == nullptr )
      {
        _db_map.erase(itr);
      }
    }
    
    PREFIXDB_LOG_MESSAGE("Attach Prefix: " << prefix)
    if ( req->opendb )
    {
      PREFIXDB_LOG_BEGIN("Open attached Prefix: " << prefix)
      ready &= nullptr != this->prefix_(prefix, false);
      PREFIXDB_LOG_END("Open attached Prefix: " << prefix)
    }
    else
      ready &= true;
  }

  if ( cb!=nullptr )
  {
    auto res = std::make_unique<response::attach_prefixes>();
    res->status = ready
      ? common_status::OK
      : common_status::CreatePrefixFail;
    cb(std::move(res));
  }
}

void multidb::detach_prefixes( request::detach_prefixes::ptr req, response::detach_prefixes::handler cb)
{
  for (const auto& prefix : req->prefixes )
  {
    if ( auto db = this->prefix_(prefix, false) )
    {
      auto req1 = std::make_unique<request::detach_prefixes>();
      req1->prefixes.push_back(prefix);
      req1->deny_timeout_s = req->deny_timeout_s;
      db->detach_prefixes(
        std::move(req1),
        [db, prefix](response::detach_prefixes::ptr){
          /*захватываем db, на случай если detach_prefixes работает асинхронно*/
          PREFIXDB_LOG_MESSAGE("Delete database for prefix '" << prefix << "'")
        });

      std::lock_guard<std::mutex> lk(_mutex);
      //_self_slave_prefixes.erase(prefix);
      if ( req->deny_timeout_s == 0)
      {
        _db_map.erase( prefix );
      }
      else
      {
        _db_map[prefix]=nullptr;
        std::weak_ptr<multidb> wthis = this->shared_from_this();
        _workflow->safe_post(
          std::chrono::seconds(req->deny_timeout_s),
          _owner.wrap([wthis,prefix]()->bool
          {
            if ( auto pthis = wthis.lock() )
            {
              std::lock_guard<std::mutex> lk2(pthis->_mutex);
              auto itr = pthis->_db_map.find(prefix);
              if ( itr != pthis->_db_map.end() && itr->second == nullptr )
              {
                pthis->_db_map.erase(itr);
              }
            }

            return false;
          }, nullptr)
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

void multidb::compact_prefix( request::compact_prefix::ptr req, response::compact_prefix::handler cb)
{
  if ( req->prefix.empty() )
  {
    bool result = this->compact();
    if (cb!=nullptr)
    {
      auto res = std::make_unique<response::compact_prefix>();
      res->status = result? common_status::OK : common_status::CompactFail;
      cb( std::move(res) );
    }
  }
  else if ( auto db = this->prefix_(req->prefix, false) )
  {
    db->compact_prefix( std::move(req), std::move(cb) );
  }
  else
  {
    prefix_not_found<response::compact_prefix>( std::move(req), std::move(cb) );
  }
}

void multidb::create_snapshot( request::create_snapshot::ptr req, response::create_snapshot::handler cb)
{
  if ( auto db = this->prefix_(req->prefix, false) )
  {
    db->create_snapshot( std::move(req), std::move(cb) );
  }
  else
  {
    prefix_not_found<response::create_snapshot>( std::move(req), std::move(cb) );
  }
}

void  multidb::release_snapshot( request::release_snapshot::ptr req, response::release_snapshot::handler cb)
{
  if ( auto db = this->prefix_(req->prefix, false) )
  {
    db->release_snapshot( std::move(req), std::move(cb) );
  }
  else
  {
    prefix_not_found<response::release_snapshot>( std::move(req), std::move(cb) );
  }
}

void multidb::stop()
{
  _owner.reset();
  if ( _workflow )
  {
    _workflow->release_timer(_archive_timer);
    _workflow->release_timer(_backup_timer);
    _workflow->release_timer(_compact_timer);
    _workflow->release_timer(_prefix_reqester);
  }

  std::lock_guard<std::mutex> lk(_mutex);
  _workflow = nullptr;
  if ( _factory )
  {
    PREFIXDB_LOG_BEGIN("STOP DB...")
    _factory = nullptr;
    for (auto& db : _db_map)
    {
      if ( db.second!= nullptr )
        db.second->stop();
      db.second=nullptr;
    }
    _db_map.clear();
    PREFIXDB_LOG_END("STOP DB!")
  }
}

bool multidb::backup()
{
  PREFIXDB_LOG_BEGIN("DB Backup...")
  if ( !::boost::filesystem::exists(_opt.backup.path) )
  {
    ::boost::system::error_code ec;
    ::boost::filesystem::create_directory(_opt.backup.path, ec);
    if (ec)
    {
      PREFIXDB_LOG_ERROR("Create directory fail (backup)'" << _opt.backup.path << "'" << ec.message() );
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
        // При неудачном бэкапе директория перемещается
        this->close_prefix_( prefix );
        db = this->prefix_(prefix, false);
        if ( db != nullptr )
        {
          // При повторном открытии создается полный бэкап (на пустой директории)
          result = db->backup();
        }
      }
      count += result;
    }
  }
  PREFIXDB_LOG_BEGIN("DB Backup DONE! " << count  )
  return count == prefixes.size();
}

bool multidb::compact()
{
  PREFIXDB_LOG_BEGIN("Compact the underlying storage for all prefixes ")
  auto prefixes = this->all_prefixes_();
  bool result = true;
  for ( const std::string& prefix: prefixes)
  {
    if ( auto db = this->prefix_(prefix, false) )
    {
      PREFIXDB_LOG_BEGIN("Compact the underlying storage for the prefix '" << prefix << "'")
      result &= db->compact();
      PREFIXDB_LOG_END("Compact the underlying storage for the prefix '" << prefix << "'")
    }
  }
  PREFIXDB_LOG_END("Compact the underlying storage for all prefixes ")
  return result;
}

bool multidb::restore()
{
  if ( !::boost::filesystem::is_directory(this->_opt.restore.path) )
  {
    PREFIXDB_LOG_ERROR( "Restore FAIL: '" << this->_opt.restore.path << "' is not directory" )
    return false;
  }

  if ( ::boost::filesystem::exists(this->_opt.path) )
  {
    ::boost::system::error_code ec;
    std::string bakpath = this->_opt.path + "_" + time_string();
    ::boost::filesystem::rename(this->_opt.path, bakpath, ec );
    if ( ec )
    {
      PREFIXDB_LOG_ERROR( "Rename old multidb FAIL: '" << this->_opt.restore.path << "' -> '" << bakpath << "'" )
      return false;
    }

    ec.clear();
    ::boost::filesystem::create_directory(this->_opt.path, ec);
    if ( ec )
    {
      PREFIXDB_LOG_ERROR( "Create directory FAIL (resore): '" << this->_opt.path << "'" )
      return false;
    }
  }

  bool fail = false;
  auto prefixes = scan_dir(_opt.restore.path, fail);
  PREFIXDB_LOG_MESSAGE("Префиксов найдено: " << prefixes.size() << " в " << _opt.restore.path)
  if (fail) return false;

  bool result = true;
  for ( const std::string& prefix: prefixes)
  {
    PREFIXDB_LOG_MESSAGE("Востановление для " << prefix)
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
      PREFIXDB_LOG_ERROR("Create directory fail '" << path << "'" << ec.message() );
      return false;
    }
  }

  bool fail = false;
  auto dirs = scan_dir(path, fail);
  if ( _opt.archive.depth <= dirs.size() )
  {
    std::sort(dirs.begin(), dirs.end() );
    std::for_each( dirs.begin(), dirs.begin() + static_cast<std::ptrdiff_t>( dirs.size() - _opt.archive.depth ) , [path](const std::string& name)
    {
      auto dir = path + '/' + name;
      std::string message;
      if ( !file::remove(dir, message) )
      {
        PREFIXDB_LOG_ERROR("Delete old archive '" << path << "': " << message)
      }
    });
  }

  path += "/" + time_string();
  ::boost::system::error_code ec;
  if( !::boost::filesystem::create_directory( path, ec) )
  {
    PREFIXDB_LOG_ERROR("Create dir " << path << " FAIL: " << ec.message() );
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
  if ( _workflow == nullptr )
    return;

  _workflow->release_timer(_archive_timer);
  if ( _opt.archive.enabled && !_opt.archive.path.empty() )
  {
    _archive_timer = _workflow->create_timer(
      _opt.archive.start_time,
      std::chrono::seconds( _opt.archive.period_s ),
      _owner.wrap([this]()->bool { this->archive(); return true; }, nullptr),
      ::wflow::expires_at::before
    );
  }
}

bool multidb::preopen_(const std::string& path, bool create_if_missing)
{
  bool fail = false;
  auto dirs = scan_dir(path, fail);
  if (fail)
  {
    PREFIXDB_LOG_FATAL("Directory " << path << " is missing");
    return false;
  }

  PREFIXDB_LOG_BEGIN("Pre open prefixes ...")
  for (auto name: dirs)
  {
    PREFIXDB_LOG_MESSAGE("Pre open prefix '" << name << "'...")
    if ( nullptr == this->prefix_(name, create_if_missing) )
    {
      PREFIXDB_LOG_WARNING("Pre open prefix FAIL")
    }
  }
  PREFIXDB_LOG_END("Pre open prefixes")
  return true;
}

void multidb::configure_compact_timer_()
{
  if ( _workflow == nullptr )
    return;

  _workflow->release_timer(_compact_timer);
  auto c = _opt.compact;
  if ( c.enabled)
  {
    _compact_timer = _workflow->create_timer(
      c.start_time,
      std::chrono::seconds( c.period_s ),
      _owner.wrap([this]() { this->compact(); return true; }, nullptr),
      ::wflow::expires_at::before
    );
  }
}

void multidb::configure_backup_timer_()
{
  if ( _workflow == nullptr )
    return;
  
  _workflow->release_timer(_backup_timer);

  if ( _opt.backup.enabled &&  !_opt.backup.path.empty() )
  {
    PREFIXDB_LOG_MESSAGE("Backup timer start '" << _opt.backup.start_time  << "' ws period " << _opt.backup.period_s << " second")
    _backup_timer = _workflow->create_timer(
      _opt.backup.start_time,
      std::chrono::seconds( _opt.backup.period_s ),
      _owner.wrap([this]() { this->backup(); return true; }, nullptr),
      ::wflow::expires_at::before
    );
  }
  
}

void multidb::configure_prefix_reqester_()
{
  if ( _workflow == nullptr )
    return;

  _workflow->release_timer(_prefix_reqester);

  if ( !_opt.slave.enabled && !_opt.initial_load.enabled )
    return;

  if( _opt.initial_load.enabled )
  {
    PREFIXDB_LOG_MESSAGE("Get All Prefixes for initial load")
    auto gap = std::make_unique<request::get_all_prefixes>();
    gap->writable_only = _slave_writable_only;
    _opt.initial_load.remote->get_all_prefixes(
      std::move(gap),
      std::bind( &multidb::get_all_prefixes_generator_, this, std::placeholders::_1)
    );
  }

  if ( _opt.slave.enabled )
  {
    std::weak_ptr<iprefixdb> wprefixdb = _opt.slave.master;
    _prefix_reqester = _workflow->create_requester<request::get_all_prefixes, response::get_all_prefixes>
    (
      std::chrono::milliseconds( _opt.slave.query_prefixes_timeout_ms ),
      [wprefixdb](request::get_all_prefixes::ptr req, response::get_all_prefixes::handler callback)
      {
        auto pprefixdb = wprefixdb.lock();
        if (pprefixdb==nullptr)
          return false;
        pprefixdb->get_all_prefixes(std::move(req), callback);
        return true;
      },
      std::bind( &multidb::get_all_prefixes_generator_, this, std::placeholders::_1)
    );
  }
}

request::get_all_prefixes::ptr multidb::get_all_prefixes_generator_(response::get_all_prefixes::ptr res)
{
  if ( res == nullptr )
  {
    auto gap = std::make_unique<request::get_all_prefixes>();
    gap->writable_only = _slave_writable_only;
    return gap;
  }

  /*
  auto preflist = this->all_prefixes_();
  std::set<std::string> prefset( preflist.begin(), preflist.end() );
  */
  std::set<std::string> prefset;
  {
    std::lock_guard<std::mutex> lk(_mutex);
    prefset=_master_prefixes;
  }
  
  if ( res->status == common_status::OK)
  {
    PREFIXDB_LOG_MESSAGE("Prefix list from master: " << res->prefixes.size() )
    for ( const auto& x : res->prefixes )
    {
      if ( x.empty() )
      {
        PREFIXDB_LOG_WARNING("get_all_prefixes: empty prefix")
        continue;
      }

      if ( this->allowed_for_slave_(x) )
      {
        // PREFIXDB_LOG_MESSAGE("Check prefix '" << x << "' from master")
        this->prefix_(x, true);
        prefset.erase(x);
        
        std::lock_guard<std::mutex> lk(_mutex);
        if ( _master_prefixes.insert(x).second)
        {
          PREFIXDB_LOG_MESSAGE("New prefix '" << x << "' from master")
        }
      }
    }

    if ( !prefset.empty() )
    {
      auto preq = std::make_shared<request::detach_prefixes>();
      preq->deny_timeout_s = 0;
      preq->prefixes.reserve(prefset.size());
      for ( auto& x : prefset )
      {
        {
          std::lock_guard<std::mutex> lk(_mutex);
          _master_prefixes.erase(x);
        }
        PREFIXDB_LOG_WARNING("Removed prefix '" << x << "' from master")
        preq->prefixes.push_back( std::move(x) );
      }

      PREFIXDB_LOG_BEGIN("Detach removed prefixes from master...")
      this->detach_prefixes( std::make_unique<request::detach_prefixes>(*preq), nullptr );
      PREFIXDB_LOG_END("Detach removed prefixes from master. Done!")
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
  return result;
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
multidb::prefixdb_ptr multidb::prefix_(const std::string& prefix,  bool create_if_missing)
{
  if ( prefix.empty() )
    return nullptr;

  std::lock_guard<std::mutex> lk(_mutex);

  if ( _factory == nullptr )
  {
    PREFIXDB_LOG_FATAL("multidb is not configured!")
    return nullptr;
  }

  prefixdb_ptr result = nullptr;

  auto itr = _db_map.find(prefix);
  if ( itr != _db_map.end() )
    return itr->second;

  if ( _max_prefixes!=0 && _db_map.size() >= _max_prefixes )
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
    PREFIXDB_LOG_MESSAGE("Open new prefix: " << prefix)
    if ( _opt.slave.enabled )
    {
      //_self_slave_prefixes.insert(prefix);
      PREFIXDB_LOG_WARNING("This prefix created on slave: " << prefix)
    }
    _db_map.insert(itr, std::make_pair(prefix, db));
    db->start();
    return db;
  }

  // Запоминаем, чтобы не создавать заново
  _db_map[prefix] = nullptr;
  return nullptr;
}


bool multidb::allowed_for_slave_(const std::string& prefix) const
{
  if ( !_slave_allowed_prefixes.empty() ) 
  {
    if ( !std::binary_search(_slave_allowed_prefixes.begin(), _slave_allowed_prefixes.end(), prefix) )
      return false;
  }
  
  if ( !_slave_denied_prefixes.empty() ) 
  {
    if ( std::binary_search(_slave_denied_prefixes.begin(), _slave_denied_prefixes.end(), prefix) )
      return false;
  }
  
  return true;
}

bool multidb::is_writable_(const std::string& prefix) const
{
  if ( !_writable_prefixes.empty() ) 
  {
    if ( !std::binary_search(_writable_prefixes.begin(), _writable_prefixes.end(), prefix) )
      return false;
  }
  
  if ( !_readonly_prefixes.empty() ) 
  {
    if ( std::binary_search(_readonly_prefixes.begin(), _readonly_prefixes.end(), prefix) )
      return false;
  }
  
  return true;
}


template<typename Res, typename ReqPtr, typename Callback>
bool multidb::is_writable_(const ReqPtr& req, const Callback& cb) const
{
  if (!this->is_writable_(req->prefix) )
    return send_error<common_status::PrefixReadonly, Res>(std::move(req), std::move(cb) );
  return true;
}

template<typename Res, typename ReqPtr, typename Callback>
bool multidb::check_prefix_(const ReqPtr& req, const Callback& cb, bool is_writable ) const
{
  if ( req->prefix.empty() )
    return send_error<common_status::EmptyPrefix, Res>(std::move(req), std::move(cb) );

  if ( is_writable && !this->is_writable_<Res>(req, cb) )
    return false;
  
  if ( _prefix_size_limit!=0 && req->prefix.size() > _prefix_size_limit )
    return send_error<common_status::PrefixLengthExceeded, Res>(std::move(req), std::move(cb) );

  return true;
}

template<typename Res, typename ReqPtr, typename Callback>
bool multidb::check_fields_(const ReqPtr& req, const Callback& cb, bool is_writable) const
{
  if ( empty_fields<Res>(req, cb) )
    return false;

  if ( !this->check_prefix_<Res>(req, cb, is_writable) )
    return false;

  if ( _keys_per_req!=0 && req->fields.size() > _keys_per_req )
    return send_error<common_status::TooManyKeys, Res>(std::move(req), std::move(cb) );

  if ( _value_size_limit==0 || _key_size_limit==0 )
    return true;

  for (const auto& f : req->fields)
  {
    if ( f.first.size() > _key_size_limit)
      return send_error<common_status::KeyLengthExceeded, Res>(std::move(req), std::move(cb) );

    if ( f.second.size() > _value_size_limit)
      return send_error<common_status::ValueLengthExceeded, Res>(std::move(req), std::move(cb) );
  }
  return true;
}

}}
