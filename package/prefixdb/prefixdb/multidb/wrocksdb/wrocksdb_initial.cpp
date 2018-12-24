
#include "wrocksdb_initial.hpp"
#include "since_reader.hpp"
#include "../aux/base64.hpp"

#include <prefixdb/logger.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/wfc_exit.hpp>


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlong-long"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <rocksdb/db.h>
#include <rocksdb/utilities/backupable_db.h>
#include <rocksdb/iterator.h>
#include <rocksdb/write_batch.h>
#include <rocksdb/utilities/backupable_db.h>
#include <rocksdb/utilities/db_ttl.h>

#pragma GCC diagnostic pop

#include <ctime>
#include <fstream>


namespace wamba{ namespace prefixdb {

wrocksdb_initial::wrocksdb_initial(std::string name, const initial_config& opt, db_type& db)
  : _name(name)
  , _opt(opt)
  , _db(db)
{
}


void wrocksdb_initial::stop()
{
  _owner.reset();
}

void wrocksdb_initial::load(std::function<void(size_t)> ready)
{
  PREFIXDB_LOG_BEGIN("Start Initial load..." )
  auto req = std::make_unique<request::create_snapshot>();
  req->prefix = this->_name;
  req->release_timeout_s = 3600 * 24;
  std::weak_ptr<wrocksdb_initial> wthis = this->shared_from_this();
  
  auto create_snapshot_handler = [wthis, ready](response::create_snapshot::ptr res)
  {
    if ( res!=nullptr && res->status==common_status::OK )
    {
      if (auto pthis = wthis.lock() )
      {
        PREFIXDB_LOG_MESSAGE("Snapshot №" << res->snapshot << " created on master " << pthis->_name);
        pthis->query_initial_range_(res->snapshot, "", true, std::bind(ready, res->last_seq + 1) );
      }
    }
    else
    {
      if ( res!=nullptr)
      {
        PREFIXDB_LOG_FATAL("Initial load FAIL: create_snapshot status=" << res->status)
      }
      else
      {
        PREFIXDB_LOG_ERROR("Initial load: create_snapshot NOT support. (Load from old version?)")
        if (auto pthis = wthis.lock() )
          pthis->query_initial_range_(0, "", true, std::bind(ready, 0) );
      }
    }
  };
  
  _opt.remote->create_snapshot( std::move(req), _owner.callback( std::move(create_snapshot_handler) ));
}

void wrocksdb_initial::query_initial_range_(size_t snapshot, const std::string& from, bool beg, std::function<void()> ready)
{
  auto req = std::make_unique<request::range>();
  req->prefix = _name;
  req->snapshot = snapshot;
  req->beg = beg;
  req->from = from;
  req->offset = 0;
  req->limit = _opt.initial_range;
  std::weak_ptr<wrocksdb_initial> wthis = this->shared_from_this();
  PREFIXDB_LOG_BEGIN("Initial load query range prefix: " << _name << ", from: '" << from 
                      << "', limit: " << _opt.initial_range << ", snapshot: " << snapshot  )
  std::string lastkey = from;
  _opt.remote->range( std::move(req), _owner.callback([wthis, snapshot, lastkey, ready](response::range::ptr res) mutable
  {
    if (auto pthis = wthis.lock() )
    {
      PREFIXDB_LOG_END("Initial load query range prefix: " << pthis->_name )
      if ( res==nullptr )
      {
        PREFIXDB_LOG_ERROR("Initial load (range): " << pthis->_name << ": Bad Gateway")
      }
      else if (res->status != common_status::OK)
      {
        PREFIXDB_LOG_FATAL("Initial load (range) FAIL: " << pthis->_name << ": " << res->status)
      }
      else if (!res->fields.empty())
      {
        lastkey = res->fields.back().first;
      }
      
      if (res==nullptr || !res->fin )
        pthis->query_initial_range_(snapshot, lastkey, false, ready);
      
      if ( res==nullptr )
        return;
    
      if ( !pthis->_opt.use_setnx )
      {
        PREFIXDB_LOG_BEGIN("Initial load: " << pthis->_name << " write recived range "<< res->fields.size() )
        rocksdb::WriteBatch batch;

        for ( const auto& field : res->fields)
          batch.Put( field.first, field.second );

        rocksdb::WriteOptions wo;
        wo.disableWAL = pthis->_opt.disableWAL;
        pthis->_db.Write( wo, &batch); 
        PREFIXDB_LOG_END("Initial load: " << pthis->_name << " write recived range "<< res->fields.size() )
      }
      else
      {
        PREFIXDB_LOG_BEGIN("Initial load: " << pthis->_name << " setnx "<< res->fields.size() )
        auto req = std::make_unique<request::setnx>();
        req->prefix = pthis->_name;
        req->fields.reserve(res->fields.size());
        for ( const auto& field : res->fields)
          req->fields.emplace_back( field.first, field.second );
        pthis->_opt.local->setnx(std::move(req), nullptr);
        PREFIXDB_LOG_END("Initial load: " << pthis->_name << " setnx "<< res->fields.size() )
      }
      
      if ( res->fin )
      {
        auto req = std::make_unique<request::release_snapshot>();
        req->prefix = pthis->_name;
        req->snapshot = snapshot; 
        PREFIXDB_LOG_BEGIN("Release snapshot " << snapshot << " " << pthis->_name)
        
        pthis->_opt.remote->release_snapshot(std::move(req), pthis->_owner.callback([wthis, ready](response::release_snapshot::ptr)
        {
          if (auto pthis = wthis.lock() )
          {
            PREFIXDB_LOG_END("Release snapshot " << pthis->_name)
            if (ready) ready();
          }
        }));
      }
    }
  }));
}


}}
