
#include "wrocksdb_slave.hpp"
#include "since_reader.hpp"
#include "../aux/base64.hpp"

#include <prefixdb/logger.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/wfc_exit.hpp>


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wlong-long"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wgnu-redeclared-enum"
#endif
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

wrocksdb_slave::wrocksdb_slave(std::string name,  std::string path, const slave_config& opt, db_type& db)
  : _name(name)
  , _path(path)
  , _opt(opt)
  , _db(db)
{
  _log_parser = std::make_shared<since_reader>();
}


void wrocksdb_slave::start(size_t last_sn )
{
  if ( last_sn!=0 )
    this->write_sequence_number_(last_sn);
  this->start();
}

void wrocksdb_slave::start()
{
  _last_update_time = 0;
  _update_counter  = 0;
  _current_differens  = 0;
  _last_sequence  = 0;
  _lost_counter = 0;
  this->start_();
}

void wrocksdb_slave::start_()
{
  if ( !_opt.enabled )
    return;

  if ( _opt.master==nullptr )
  {
    PREFIXDB_LOG_WARNING("Slave '" << _name << "' not running. enabled==true but target not set")
    return;
  }

  PREFIXDB_LOG_BEGIN("Start Slave '" << _name << "' ")
  
  this->create_updates_requester_();
  this->create_diff_timer_();
  this->create_seq_timer_();
  PREFIXDB_LOG_END("Start Slave '" << _name << "' ")
  std::lock_guard<mutex_type> lk(_mutex);
  is_started = true;
}


void wrocksdb_slave::stop()
{
  std::lock_guard<mutex_type> lk(_mutex);
  is_started = false;
  if ( !_opt.enabled )
    return;
  
  _opt.timer->release_timer(_slave_timer_id);
  _opt.timer->release_timer(_seq_timer_id);
  _opt.timer->release_timer(_diff_timer_id);
}


void wrocksdb_slave::create_updates_requester_()
{
  auto preq = std::make_shared<request::get_updates_since>();
  preq->seq = 0;
  preq->prefix  = _name;
  preq->limit = _opt.log_limit_per_req;
  
  uint64_t seq = this->read_sequence_number_();
  
  if ( seq == static_cast<uint64_t>(-1) )
  {
    PREFIXDB_LOG_FATAL("Invalid sequence number. Replication is not possible. You must synchronize the database: " << this->_name )
  }
  else if ( seq == 0)
  {
    preq->seq = _db.GetLatestSequenceNumber() + 1;
  }
  else
  {
    preq->seq = seq;
  }
  
  std::weak_ptr<iprefixdb> wprefixdb = _opt.master;
  _opt.timer->release_timer(_slave_timer_id);
  _slave_timer_id = _opt.timer->create_requester<request::get_updates_since, response::get_updates_since>
  (
    _opt.start_time,
    std::chrono::milliseconds(_opt.pull_timeout_ms),
    [wprefixdb](request::get_updates_since::ptr req, response::get_updates_since::handler callback) -> bool
    {
      auto pprefixdb = wprefixdb.lock();
      if ( pprefixdb == nullptr )
        return false;
      pprefixdb->get_updates_since( std::move(req), callback);
      return true;
    },
    std::bind(&wrocksdb_slave::updates_generator_, this->shared_from_this(), std::placeholders::_1, preq)
  );
}

request::get_updates_since::ptr wrocksdb_slave::updates_generator_(
  std::weak_ptr<wrocksdb_slave> wthis,
  response::get_updates_since::ptr res, 
  std::shared_ptr<request::get_updates_since> preq
)
{
  auto pthis = wthis.lock();
  if ( pthis == nullptr)
    return nullptr;
  
  if ( res == nullptr )
    return std::make_unique<request::get_updates_since>(*preq);

  if ( res->status != common_status::OK || preq->seq > (res->seq_final + 1 ))
  {
    PREFIXDB_LOG_FATAL( "Slave replication error. Invalid master responce for '" << pthis->_name << "'"
      << " need sequence == " << preq->seq << ", but last acceptable == " << res->seq_final 
      << " status="<<res->status) 
    return nullptr;
  }

  if ( res->logs.empty() )
    return nullptr;

  if ( preq->seq!=0  )
  {
    auto diff = static_cast<std::ptrdiff_t>( res->seq_first - preq->seq );
    if ( diff > pthis->_opt.acceptable_loss_seq )
    {
      PREFIXDB_LOG_FATAL( "Slave not acceptable loss sequence '" << pthis->_name << "': " 
                        << diff << " request segment=" << preq->seq << " response=" << res->seq_first)
      return nullptr;
    }
    else if ( diff > 0)
    {
      PREFIXDB_LOG_WARNING( "Slave not acceptable loss sequence '" << pthis->_name << "' : " 
                          << diff << " request segment=" << preq->seq << " response=" << res->seq_first );
      pthis->_lost_counter += static_cast<size_t>(diff);
    } 
    else if ( diff < 0 )
    {
      PREFIXDB_LOG_TRACE( "re-entry sequences'"<< pthis->_name<<"': " << diff 
                  << " request segment=" << preq->seq << " response=" << res->seq_first );
    }
    
    //this->_current_differens = diff;
  }
      
  //this->_current_differens = res->seq_final - res->seq_first;
  
  pthis->logs_parser_(res);
  auto batch = pthis->_log_parser->detach();
  //size_t sn = res->seq_last + 1;
  uint64_t sn = pthis->_log_parser->get_next_seq_number();
  pthis->_current_differens = static_cast<std::ptrdiff_t>( res->seq_final - (sn - 1) );
  pthis->_last_sequence = sn;
  // batch->Put("~slave-last-sequence-number~", ::rocksdb::Slice( reinterpret_cast<const char*>(&sn), sizeof(sn) ));
  pthis->write_sequence_number_( static_cast<uint64_t>(-1) );
  {
    std::lock_guard<mutex_type> lk(pthis->_mutex);
    if ( pthis->is_started )
    {
      ::rocksdb::WriteOptions wo;
      wo.disableWAL = pthis->_opt.disableWAL;
      pthis->_db.Write( wo, batch.get() );
    }
  }
  pthis->write_sequence_number_(sn);

  preq->seq = sn;
  
  if ( res->seq_last == res->seq_final )
    return nullptr;
      
  return std::make_unique<request::get_updates_since>(*preq);
}

void wrocksdb_slave::logs_parser_( response::get_updates_since::ptr& res)
{
  auto count = res->seq_first;
  for (auto& log : res->logs ) try
  {
    std::vector<char> binlog;
    binlog.reserve(512);
    size_t tail = 0;
    // decode64 модифицирует log (костыль для строго буста)
    decode64(log.begin(), log.end(), std::inserter(binlog,binlog.end()), tail );
    binlog.resize( binlog.size() - tail);
    ++count;
    this->_log_parser->parse( binlog );
    this->_update_counter++;
  }
  catch(...)
  {
    res->seq_last = count - 1;
    this->_log_parser.reset();
    PREFIXDB_LOG_ERROR("attempt to decode a value not in base64 char set: [" << log << "]" )
    break;
  }
}

void wrocksdb_slave::create_diff_timer_()
{
  _diff_timer_id = _opt.timer->release_timer(_diff_timer_id);
  if ( this->_opt.wrn_log_diff_seq !=0 )
  {
    _diff_timer_id = _opt.timer->create_timer(
      std::chrono::milliseconds(_opt.wrn_log_timeout_ms),
      [this]()->bool
      {
        if ( this->_current_differens > this->_opt.wrn_log_diff_seq )
        {
          PREFIXDB_LOG_WARNING("Slave replication too big difference '" << this->_name << "': " << this->_current_differens << "(wrn:" << this->_opt.wrn_log_diff_seq << ")");
        }
        
        if ( _lost_counter > 0)
	{
	  PREFIXDB_LOG_WARNING("Lost segments '"<< this->_name<<"': " << _lost_counter)
	}
        return true;
      }
    );
  }
}

void wrocksdb_slave::create_seq_timer_()
{
  _seq_timer_id = _opt.timer->release_timer(_seq_timer_id);
  auto update_counter = std::make_shared< std::atomic<size_t> >(0);
  if ( _opt.seq_log_timeout_ms != 0 )
  {
    auto last_time = std::make_shared< std::atomic<time_t> >( time(0) );
    std::weak_ptr<wrocksdb_slave> wthis=this->shared_from_this();
    _seq_timer_id = _opt.timer->create_timer(
      std::chrono::milliseconds( _opt.seq_log_timeout_ms ),
      [wthis, last_time]()->bool
      {
        auto pthis = wthis.lock();
        if (pthis==nullptr)
          return false;
        
        time_t span = std::time(0) - last_time->load();
        if ( pthis->_update_counter == 0)
        {
          PREFIXDB_LOG_MESSAGE("No updates for '" << pthis->_name << "' in the last " << span 
                  << " seconds. Next seq №" << pthis->_last_sequence );
        }
        else
        {
          PREFIXDB_LOG_MESSAGE( pthis->_update_counter.load() << " updates for '" << pthis->_name << "' in the last " << span << " seconds. Next seq №" << pthis->_last_sequence );
          last_time->store( std::time(0) );
          pthis->_update_counter = 0;
        }
        return true;
      }
    );
  }
}

void wrocksdb_slave::write_sequence_number_(uint64_t seq)
{
  auto path = _path + "/slave-sequence-number";
  std::ofstream ofs(path);
  ofs << seq;
  ofs.flush();
}

uint64_t wrocksdb_slave::read_sequence_number_()
{
  uint64_t seq = 0;
  auto path = _path + "/slave-sequence-number";
  std::ifstream ofs(path);
  ofs >> seq;
  return seq;
}

}}
