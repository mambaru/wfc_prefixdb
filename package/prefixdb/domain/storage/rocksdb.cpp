
#include "rocksdb.hpp"
//#include "persistent_value.hpp"
#include "merge/merge.hpp"
#include "merge/merge_json.hpp"

#include <wfc/logger.hpp>
#include <wfc/json.hpp>
#include <rocksdb/db.h>
#include <rocksdb/write_batch.h>
#include <rocksdb/iterator.h>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <boost/filesystem.hpp>

namespace wamba{ namespace prefixdb {

namespace {
  inline std::string& get_key(std::string& key) {return key;}
  inline std::string& get_key( std::pair<std::string, std::string>& field) {return field.first;}
}

rocksdb::rocksdb( std::string name, const rocksdb_config conf,  db_type* db)
  : _name(name)
  , _conf(conf)
  , _db(db)
  , _slave_timer_id(-1)
  
{
}


void rocksdb::start( ) 
{
  //::iow::io::timer_options topt;
  //topt.start_time = _conf.slave.start_time;
  //topt.delay_ms = 1000; /*_conf.slave.pull_timeout_ms;*/
  //topt.expires_from_now = true;
  
  //  _master = _conf.slave.master;
  if ( _conf.slave.master!=nullptr && _conf.slave.enabled )
  {
    this->create_slave_timer_();
  }
  
  
  /*
  auto preq = std::make_shared<request::get_updates_since>();
  preq->seq = 0;
  preq->prefix  = this->_name;
  preq->limit = this->_conf.slave.log_limit_per_req;
  _timer_id = _conf.slave.timer->create_requester<request::get_updates_since, response::get_updates_since>
  (
    _conf.slave.start_time,
    std::chrono::milliseconds(_conf.slave.pull_timeout_ms),
    _master,
    &iprefixdb::get_updates_since,
    [this, preq](response::get_updates_since::ptr res) -> request::get_updates_since::ptr
    {
            DEBUG_LOG_MESSAGE("Tick " << (res == nullptr) << " rocks STUB " << this->_name )
      if ( res == nullptr ) 
        return std::make_unique<request::get_updates_since>();
      else
        return nullptr;
      
    }
  );*/
  
  

  // if ( _conf.slave.enabled )
  // if ( _conf.path == "./rocksdb2")
 
 
  /*
  if ( auto master = _conf.slave.master )
  {
    if ( _conf.slave.enabled )
    {
      std::weak_ptr<rocksdb> wthis = this->shared_from_this();
      auto hdl = [wthis, master]( ::iow::io::timer::handler_callback handler)
      {
        if ( auto pthis = wthis.lock() )
        {
          // DEBUG_LOG_MESSAGE("rocksdb::rocksdb timer " << pthis->_name)
          // handler();
          auto req = std::make_unique<request::get_updates_since>();
          req->seq = 0;
          req->prefix  = pthis->_name;
          req->limit = 10;
          std::string value;
          ::rocksdb::Status status = pthis->_db->Get( ::rocksdb::ReadOptions(), "~slave-last-sequence-number~", &value);
          if ( status.ok() )
          {
            req->seq = *( reinterpret_cast<const size_t*>( value.data() ) );
          }
          else
          {
            DEBUG_LOG_MESSAGE("Get ~slave-last-sequence-number~ : " << status.ToString() )
          }
          DEBUG_LOG_MESSAGE("request get_updates_since seq=" << req->seq )
          
          master->get_updates_since( std::move(req), [handler, wthis](response::get_updates_since::ptr res)
          {
            bool delay = true;
            if ( auto pthis = wthis.lock() )
            {
              DEBUG_LOG_BEGIN("response get_updates_since res->seq_first=" << res->seq_first << " seq_last=" << res->seq_last << " seq_final=" << res->seq_final )
              if ( res->logs.size() )
              {
                size_t sn = res->seq_last + 1;
                delay = !( sn < res->seq_final );
                ::rocksdb::WriteBatch batch;
                batch.Put("~slave-last-sequence-number~", ::rocksdb::Slice( reinterpret_cast<const char*>(&sn), sizeof(sn) ));
                for (const auto& log : res->logs )
                {
                  //DEBUG_LOG_MESSAGE("\n" << log  )
                  batch.PutLogData( log );
                }
                pthis->_db->Write( ::rocksdb::WriteOptions(), &batch);
              }
              DEBUG_LOG_END("response get_updates_since " << res->logs.size() )
            }
            handler(delay);
          } );
        }
      };
      _conf.slave.timer->start(hdl, topt);
    }
  }
  */
  
  
  /*else
  {
    DEBUG_LOG_MESSAGE("rocksdb::start slave disabled " << _conf.slave.target   )
    abort();
  }*/
}

/*
void rocksdb::set_master(std::shared_ptr<iprefixdb> master)
{
  _master = master;
}
*/
namespace detail {
  using namespace rocksdb;
  struct Handler : public WriteBatch::Handler {                                                                                                                                              
    std::stringstream seen;
    virtual Status PutCF(uint32_t cf, const Slice& key,
                         const Slice& value) override {
      seen << "Put(" << cf << ", " << key.ToString() << ", " <<
              value.size() << ")";
      return Status::OK();
    }
    virtual Status MergeCF(uint32_t cf, const Slice& key,
                           const Slice& value) override {
      seen << "Merge(" << cf << ", " << key.ToString() << ", " <<
              value.size() << ")";
      return Status::OK();
    }
    virtual void LogData(const Slice& blob) override {
      const char* beg = blob.data();
      for (int i=0; i < blob.size(); i++ )
      {
        if ( beg[i] < 32 || beg[i] > 127 )
          seen << int(beg[i]) << "|";
        else
          seen << beg[i] ;
      }
      seen << std::endl;
      /*
      seen << "src=" << int(*reinterpret_cast<const uint32_t*>(beg)) << std::endl
           << "size=" << int(*reinterpret_cast<const uint32_t*>(beg + 2)) << std::endl
           << "type=" << int(*reinterpret_cast<const uint8_t*>(beg + 6)) << std::endl
           << "playload1=[" << std::string(beg + 10, beg + blob.size() - 10 ) << "]" << std::endl;
      // seen << "LogData(" << blob.ToString() << ")";
           */
    }
    virtual Status DeleteCF(uint32_t cf, const Slice& key) override {
      seen << "Delete(" << cf << ", " << key.ToString() << ")";
      return Status::OK();
    }
  };
}
void rocksdb::create_slave_timer_()
{
  auto preq = std::make_shared<request::get_updates_since>();
  preq->seq = 0;
  preq->prefix  = this->_name;
  preq->limit = this->_conf.slave.log_limit_per_req;
  
  std::string value;
  ::rocksdb::Status status = this->_db->Get( ::rocksdb::ReadOptions(), "~slave-last-sequence-number~", &value);
  if ( status.ok() )
  {
    preq->seq = *( reinterpret_cast<const size_t*>( value.data() ) );
  }
  else
  {
    DEBUG_LOG_MESSAGE("Get ~slave-last-sequence-number~ : " << status.ToString() )
  }
  DEBUG_LOG_MESSAGE("request get_updates_since seq=" << preq->seq )

  _slave_timer_id = _conf.slave.timer->create_requester<request::get_updates_since, response::get_updates_since>
  (
    _conf.slave.start_time,
    std::chrono::milliseconds(_conf.slave.pull_timeout_ms),
    _conf.slave.master,
    &iprefixdb::get_updates_since,
    [this, preq](response::get_updates_since::ptr res) -> request::get_updates_since::ptr
    {
      if ( res == nullptr )
        return std::make_unique<request::get_updates_since>(*preq);
      
      DEBUG_LOG_BEGIN("response get_updates_since size=" << res->logs.size() << " res->seq_first=" << res->seq_first << " seq_last=" << res->seq_last << " seq_final=" << res->seq_final )

      if ( !res->logs.empty() )
      {
        size_t sn = res->seq_last + 1;
        ::rocksdb::WriteBatch batch;
        batch.Put("~slave-last-sequence-number~", ::rocksdb::Slice( reinterpret_cast<const char*>(&sn), sizeof(sn) ));
        for (const auto& log : res->logs )
        {
          batch.PutLogData( log );
        }
        
        /*detail::Handler handler;
        ::rocksdb::Status status1 = batch.Iterate(&handler);*/
        ::rocksdb::Status status2 = this->_db->Write( ::rocksdb::WriteOptions(), &batch);
        DEBUG_LOG_MESSAGE("WRITED " << status2.ToString() /*<< " " << status2.ToString()*/ )
        //std::cout << std::endl << handler.seen.str() << std::endl;
        {
          std::string value;
          ::rocksdb::Status status = this->_db->Get( ::rocksdb::ReadOptions(), "inc_test", &value);
          DEBUG_LOG_MESSAGE("GET " << status.ToString() << " " << value << " " << res->logs.back() )

        }
        preq->seq = sn;
        if ( res->seq_last != res->seq_final )
        {
          return std::make_unique<request::get_updates_since>(*preq);
        }
      }
      return nullptr;
    }
  );

  
     
  /*
  auto preq = std::make_shared<request::get_updates_since>();
  preq->seq = 0;
  preq->prefix  = this->_name;
  preq->limit = this->_conf.slave.log_limit_per_req;

  std::string value;
  ::rocksdb::Status status = this->_db->Get( ::rocksdb::ReadOptions(), "~slave-last-sequence-number~", &value);
  if ( status.ok() )
  {
    preq->seq = *( reinterpret_cast<const size_t*>( value.data() ) );
  }
  else
  {
    DEBUG_LOG_MESSAGE("Get ~slave-last-sequence-number~ : " << status.ToString() )
  }
  DEBUG_LOG_MESSAGE("request get_updates_since seq=" << preq->seq )

  std::weak_ptr<iprefixdb> wmaster = _master;
  std::weak_ptr<rocksdb> wthis = this->shared_from_this();
  DEBUG_LOG_MESSAGE("create timer..." )
  auto id = _conf.slave.timer->create_timer
  (
    _conf.slave.start_time,
    std::chrono::milliseconds(_conf.slave.pull_timeout_ms),
    [wmaster, wthis, preq]( timer_handler handler )
    {
      DEBUG_LOG_MESSAGE("TATAM ..." )
      if (auto pthis = wthis.lock() )
      {
        pthis->query_updates_since_( wmaster, std::move(handler), preq );
      }
      else
      {
        handler(false);
      }
    }
  );
  DEBUG_LOG_MESSAGE("timer ready " << id << " ms " << std::chrono::milliseconds(_conf.slave.pull_timeout_ms).count() )
  */
}

void rocksdb::query_updates_since_(std::weak_ptr<iprefixdb> wmaster, timer_handler handler, request_since_ptr preq)
{
  DEBUG_LOG_MESSAGE("\t query_updates_since_ " )
  auto req = std::make_unique<request::get_updates_since>(*preq);
  std::weak_ptr<rocksdb> wthis = this->shared_from_this();
  if ( auto pmaster = wmaster.lock() )
  {
    DEBUG_LOG_MESSAGE("\t call get_updates_since... " )
    pmaster->get_updates_since( std::move(req), [wmaster, handler, wthis, preq](response::get_updates_since::ptr res)
    {
      DEBUG_LOG_MESSAGE("\t response get_updates_since... " )
      if (auto pthis = wthis.lock() )
      {
        pthis->result_handler_updates_since_(wmaster, handler, preq, std::move(res));
      }
    });
  }
  else
  {
    DEBUG_LOG_MESSAGE("\t query_updates_since_ master == nullptr!!!" )
    handler( false );
  }
}

void rocksdb::result_handler_updates_since_(std::weak_ptr<iprefixdb> master, timer_handler handler, request_since_ptr preq, response::get_updates_since::ptr res)
{
  DEBUG_LOG_BEGIN("response get_updates_since size=" << res->logs.size() << " res->seq_first=" << res->seq_first << " seq_last=" << res->seq_last << " seq_final=" << res->seq_final )
  bool fin = true;
  if ( res->logs.size() )
  {
    size_t sn = res->seq_last + 1;
    ::rocksdb::WriteBatch batch;
    batch.Put("~slave-last-sequence-number~", ::rocksdb::Slice( reinterpret_cast<const char*>(&sn), sizeof(sn) ));
    for (const auto& log : res->logs )
    {
      batch.PutLogData( log );
    }
    this->_db->Write( ::rocksdb::WriteOptions(), &batch);
    preq->seq = sn;
    if ( res->seq_last != res->seq_final )
    {
      this->query_updates_since_(master, std::move(handler), preq);
      fin = false;
    }
  }
  
  if (fin)
  {
    handler(true);
  }
  DEBUG_LOG_END("response get_updates_since " << res->logs.size() )
}


void rocksdb::close()
{
  COMMON_LOG_MESSAGE("preffix DB close " << _name)
  _db=nullptr;
}


template<merge_mode Mode, typename Res, typename ReqPtr, typename Callback>
void rocksdb::merge_(ReqPtr req, Callback cb)
{
  ::rocksdb::WriteBatch batch;
  std::string json;
  json.reserve(64);
  merge_json::serializer ser;
  for (auto& f: req->fields)
  {
    merge upd;
    upd.mode = Mode;
    upd.raw = std::move(f.second);
    json.clear();
    ser( upd, std::inserter(json, json.end()));
    //std::cout << "rocksdb::merge_ " << json << std::endl;
    batch.Merge(f.first, json);
  }
  
  this->write_batch_<Res>(batch, std::move(req), std::move(cb) );
}

template<typename Res, typename Batch, typename ReqPtr, typename Callback>
void rocksdb::write_batch_(Batch& batch, ReqPtr req, Callback cb)
{
  ::rocksdb::WriteOptions wo;
  wo.sync = req->sync;
  
  ::rocksdb::Status status = _db->Write( ::rocksdb::WriteOptions(), &batch);

  if ( cb == nullptr )
    return;

  if ( req->nores || !status.ok() )
  {
    auto res = std::make_unique<Res>();
    res->prefix = std::move(req->prefix);
    res->status = status.ok() ? common_status::OK : common_status::WriteError;
    cb( std::move(res) );
  }
  else
  {
    this->get_<Res>( std::move(req), std::move(cb) );
  }
}

template<typename Res, typename ReqPtr, typename Callback>
void rocksdb::get_(ReqPtr req, Callback cb)
{
  typedef Res response_type;
  typedef ::rocksdb::Slice slice_type;
  
  // Формируем список ключей для запроса
  std::vector<slice_type> keys;
  keys.reserve(req->fields.size() );
  for ( auto& fld: req->fields ) 
    keys.push_back( get_key(fld) );

  // Забираем значения
  std::vector<std::string> resvals;
  resvals.reserve(keys.size());
  std::vector< ::rocksdb::Status> status
    = _db->MultiGet( ::rocksdb::ReadOptions(), keys, &resvals);

  auto res = std::make_unique<response_type>();
  res->prefix = std::move(req->prefix);
  res->status = common_status::OK;
  res->fields.reserve(resvals.size());
  
  // Формируем результата
  field_pair field;
  for ( size_t i = 0; i!=resvals.size(); ++i)
  {
    field.first = std::move( get_key(req->fields[i]) );
    
    if ( req->noval )
    { // Если значения не нужны
      field.second = status[i].ok() ? "true" : "false";
    }
    else if ( status[i].ok() )
    { // Если ключ существет
      field.second = std::move(resvals[i]);
    }
    else
    { // Если ключа нет записывам null
      field.second = "null";
    }
    res->fields.push_back( std::move(field) );
  }
  
  cb( std::move(res) );
}

void rocksdb::set( request::set::ptr req, response::set::handler cb)
{
  ::rocksdb::WriteBatch batch;

  for ( const auto& field : req->fields)
  {
    batch.Put( field.first, field.second );
  }
  
  this->write_batch_<response::set>(batch, std::move(req), std::move(cb) );
}

void rocksdb::get( request::get::ptr req, response::get::handler cb)
{
  this->get_<response::get>( std::move(req), std::move(cb) );
}

void rocksdb::has( request::has::ptr req, response::has::handler cb)
{
  this->get_<response::has>( std::move(req), std::move(cb) );
}

void rocksdb::del( request::del::ptr req, response::del::handler cb) 
{
  ::rocksdb::WriteBatch batch;
  for ( auto& key : req->fields)
  {
    batch.Delete( key );
  }

  if ( req->nores || cb==nullptr)
  {
    ::rocksdb::Status status = _db->Write( ::rocksdb::WriteOptions(), &batch);
    if ( cb!=nullptr )
    {
      auto res = std::make_unique<response::del>();
      res->prefix = std::move(req->prefix);
      res->status = status.ok() ? common_status::OK : common_status::WriteError;
      cb( std::move(res) );
    }
  }
  else
  {
    this->get_<response::del>( std::move(req), [this, &batch, &cb](response::del::ptr res)
    {
      ::rocksdb::Status status = this->_db->Write( ::rocksdb::WriteOptions(), &batch);
      res->status = status.ok() ? common_status::OK : common_status::WriteError;
      cb( std::move(res) );
    });
  }
}


void rocksdb::inc( request::inc::ptr req, response::inc::handler cb) 
{
  this->merge_<merge_mode::inc, response::inc>( std::move(req), std::move(cb) );
}

void rocksdb::add( request::add::ptr req, response::add::handler cb) 
{
  this->merge_<merge_mode::add, response::add>( std::move(req), std::move(cb) );
}

void rocksdb::packed( request::packed::ptr req, response::packed::handler cb)
{  
  this->merge_<merge_mode::packed, response::packed>( std::move(req), std::move(cb) );
}


namespace {

  void tmp_trace_playload(const char *beg, const char *end);

  void tmp_trace_put(const char *beg, const char *end)
  {
    const int offset = 1;
    int size_key = int( *reinterpret_cast<const uint8_t*>(beg++) );
    std::cout << "\tPut (" << size_key << ")" << std::string( beg, beg + size_key) << "=";
    beg += size_key;
    int size_val = int( *reinterpret_cast<const uint8_t*>(beg++) );
    std::cout << "(" << size_val << ")" << std::string( beg, beg + size_val) << std::endl;
    beg += size_val;
    tmp_trace_playload(beg, end);
  }

  void tmp_trace_merge(const char *beg, const char *end)
  {
    const int offset = 1;
    int size_key = int( *reinterpret_cast<const uint8_t*>(beg++) );
    std::cout << "\tMerge " << std::string( beg, beg + size_key) << "=";
    beg += size_key;
    int size_val = int( *reinterpret_cast<const uint8_t*>(beg++) );
    std::cout << std::string( beg, beg + size_val) << std::endl;
    beg += size_val;
    tmp_trace_playload(beg, end);
  }

  void tmp_trace_del(const char *beg, const char *end)
  {
    const int offset = 1;
    int size_key = int( *reinterpret_cast<const uint8_t*>(beg++) );
    std::cout << "\tDel " << std::string( beg, beg + size_key) << "!" << std::endl;
    beg += size_key;
    tmp_trace_playload(beg, end);
  }

  /*
  void tmp_trace_playload_next(const char *beg, const char *end)
  {
    if (beg >= end) return;
    const int offset = 1;
    int size = int( *reinterpret_cast<const uint8_t*>(beg) );
    std::cout << "\t" << "(" << "X" << "," << size << ") " 
              << std::string( beg + offset, beg + offset + size)
              << std::endl;
    tmp_trace_playload_next(beg + offset + size, end);
  }*/

  void tmp_trace_playload(const char *beg, const char *end)
  {
    if (beg >= end) {
      std::cout << "END" << std::endl;
      return;
    }
    const int offset = 1;
    int type = int( *reinterpret_cast<const uint8_t*>(beg+0) );
    std::cout << "NEXT type=" << type << " size?=" << int( *reinterpret_cast<const uint16_t*>(beg) ) << std::endl;
    beg += 1;
    switch ( type )
    {
      case 0: tmp_trace_del(beg, end); break;
      case 1: tmp_trace_put(beg, end); break;
      case 2: tmp_trace_merge(beg, end); break;
    }
    /*std::cout << "\t" << "(" << type << "," << size << ") " 
              << std::string( beg + offset, beg + offset + size)
              << std::endl;
    tmp_trace_playload_next(beg + offset + size, end);
    */
  }
  
  void tmp_tarce_log( const std::string& str)
  {
    const int offset = 12;
    std::vector<char> data(str.begin(), str.end() );
    const char *beg = data.data();
    const char *end = beg + data.size();
    std::cout << "LOG (" << str.size() << "," << offset << ") " << std::endl;
    tmp_trace_playload(beg + offset, end);
    std::cout << std::endl;
  }
}

void rocksdb::get_updates_since( request::get_updates_since::ptr req, response::get_updates_since::handler cb) 
{
  auto res = std::make_unique<response::get_updates_since>();
  std::unique_ptr< ::rocksdb::TransactionLogIterator> iter;
  ::rocksdb::Status status = _db->GetUpdatesSince(req->seq, &iter, ::rocksdb::TransactionLogIterator::ReadOptions() );
  ::rocksdb::SequenceNumber cur_seq=0;
  if ( status.ok() )
  {
    if ( iter->Valid() )
    {
      res->logs.reserve(req->limit);
      bool first = true;
      while ( iter->Valid() && req->limit-- )
      {
        ::rocksdb::BatchResult batch = iter->GetBatch();
        if ( batch.sequence < req->seq )
        {
          DEBUG_LOG_MESSAGE("batch seq err " << batch.sequence << " < " << req->seq );
          // Может быть меньше запрашиваемого
          // Проверить это
          iter->Next();
          continue;
        }
        
        {
          // tmp
          std::string str  = batch.writeBatchPtr->Data();
          tmp_tarce_log(str);
          /*const char *beg = str.c_str();
          const char *end = beg + str.size();
          std::cout << "Log " << int(end - beg) << ":";
          for ( int i = 0;beg!=end && i!=13; ++beg, ++i)
            std::cout << int(*beg) << ",";
          std::cout << "|" << std::endl;
          for ( int i = 0;beg!=end && i!=100; ++beg, ++i)
            std::cout << *beg << "("<< int( *beg) << ")";
          std::cout << std::endl;
          */
          
        }
        res->logs.push_back( batch.writeBatchPtr->Data() );
        cur_seq = batch.sequence;
        //std::cout << "cur seq " << cur_seq << std::endl;
        if ( first )
        {
          DEBUG_LOG_MESSAGE("rocksdb::get_updates_since first_seq=" << batch.sequence)
          res->seq_first = cur_seq;
          first = false;
        }
        iter->Next();
      }
      res->seq_last  = cur_seq;
    }
  }
  res->seq_final = _db->GetLatestSequenceNumber();
  /*
  else
  {
    res->status = common_status::TransactLogError;
    DEBUG_LOG_MESSAGE("rocksdb::get_updates_since " << status.ToString() )
  }*/
  cb( std::move(res) );  
}


void rocksdb::range( request::range::ptr req, response::range::handler cb)
{
  DEBUG_LOG_MESSAGE("range from '" << req->from << " to '" << req->to )
  typedef ::rocksdb::Iterator iterator_type;
  typedef ::rocksdb::Slice slice_type;
  
  typedef std::shared_ptr<iterator_type> iterator_ptr;
  auto res=std::make_unique<response::range>();
  res->status = common_status::OK;
  res->prefix = std::move(req->prefix);
  res->fin = false;
  
  ::rocksdb::ReadOptions opt;
  iterator_ptr itr(_db->NewIterator(opt));
  if ( itr != nullptr )
  {
    field_pair field;
    
    // offset
    itr->Seek( req->from );
    while ( req->offset && itr->Valid() ) 
    {
      req->offset--; itr->Next(); 
    }
    
    while ( req->limit && itr->Valid() )
    {
      slice_type key = itr->key();
      
      field.first.assign(key.data(), key.data() + key.size() );
      DEBUG_LOG_MESSAGE("key " << field.first << " > " << req->to << "? " << (field.first > req->to) )
      
      if ( !req->to.empty() && field.first > req->to )
        break;
      
      if ( !req->noval )
      {
        auto val = itr->value();
        field.second.assign(val.data(), val.data() + val.size() );
      }
      res->fields.push_back(std::move(field));
      itr->Next();
      req->limit--;
    }
    
    res->fin |= req->limit!=0;
    res->fin |= !itr->Valid();
    if ( !res->fin )
    {
      itr->Next();
      res->fin = !itr->Valid() || itr->value()==req->to;
    }
  }
  cb( std::move(res) );
}


namespace {

  /*
inline bool backup_status_(const ::rocksdb::Status& s, const request::backup::ptr& req, const response::backup::handler& cb)
{
  if ( s.ok() ) return true;
  if ( cb == nullptr ) return false;
  auto res = std::make_unique< response::backup >();
  res->status = common_status::WriteError;
  if ( !req->nores &&  !req->prefixes.empty() )
  {
    res->status_map.push_back( std::make_pair( std::move(req->prefixes[0]), "false") );
  }
  cb( std::move(res) );
  return false;
}
*/

}


void rocksdb::prebackup_(bool /*compact_range*/)
{
  
}

bool rocksdb::backup()
{
  std::lock_guard<std::mutex> lk(_backup_mutex);
  
  if ( _conf.compact_before_backup )
  {
    DEBUG_LOG_BEGIN("CompactRange: " << _name )
    ::rocksdb::Status status = _db->CompactRange( ::rocksdb::CompactRangeOptions(), nullptr, nullptr);
    DEBUG_LOG_END("CompactRange: " << status.ToString() )
  }
  
  {
    ::rocksdb::Status status = _db->GarbageCollect();
    if ( status.ok() )
    {
      DEBUG_LOG_MESSAGE( "GarbageCollect for " << _name <<  ": " << status.ToString() )
    }
    else
    {
      COMMON_LOG_MESSAGE( "GarbageCollect ERROR for " << _name << ": " << status.ToString() )
    }
  }
  
  ::rocksdb::Status status = _db->CreateNewBackup();
  if ( status.ok() )
  {
    DEBUG_LOG_MESSAGE("CreateNewBackup for " << _name <<  ": " << status.ToString() )
  }
  else
  {
    COMMON_LOG_MESSAGE("Create Backup ERROR for " << _name << ": " << status.ToString() )
  }
  
  status = _db->PurgeOldBackups(5);
  if ( status.ok() )
  {
    DEBUG_LOG_MESSAGE("PurgeOldBackups(5) for " << _name <<  ": " << status.ToString() )
  }
  else
  {
    COMMON_LOG_MESSAGE("PurgeOldBackups(5) ERROR for " << _name << ": " << status.ToString() )
    return false;
  }
  return true;
}

namespace 
{
  inline bool copy_dir(
    boost::filesystem::path const & source,
    boost::filesystem::path const & destination,
    std::string& message
  )
  {
    namespace fs = boost::filesystem;
  
    try
    {
      // Check whether the function call is valid
      if( !fs::exists(source) || !fs::is_directory(source) )
      {
        std::stringstream ss;
        ss << "Source directory " << source.string() << " does not exist or is not a directory." << '\n';
        message = ss.str();
        return false;
      }
      
      if ( fs::exists(destination) )
      {
        std::stringstream ss;
        ss << "Destination directory " << destination.string() << " already exists." << '\n';
        message = ss.str();
        return false;
      }
      
      // Create the destination directory
      if( !fs::create_directory(destination) )
      {
        std::stringstream ss;
        ss << "Unable to create destination directory " << destination.string() << '\n';
        message = ss.str();
        return false;
      }
    }
    catch(fs::filesystem_error const & e)
    {
      std::stringstream ss;
      ss << e.what() << '\n';
      message = ss.str();
      return false;
    }
    
    // Iterate through the source directory
    for( fs::directory_iterator file(source); file != fs::directory_iterator(); ++file) try
    {
      fs::path current(file->path());
      if( fs::is_directory(current) )
      {
        // Found directory: Recursion
        if( !copy_dir(current, destination / current.filename(), message) )
          return false;
      }
      else
      {
        // Found file: Copy
        ::boost::filesystem::copy( current, destination / current.filename());
      }
    }
    catch(const fs::filesystem_error& e)
    {
      std::stringstream ss;
      ss << e.what() << '\n';
      message = ss.str();
      return false;
    }
    return true;
  }

  inline bool copy_dir(const std::string& from, const std::string& to, std::string& message)
  {
    return copy_dir( ::boost::filesystem::path(from),  ::boost::filesystem::path(to), message);
  }
}

bool rocksdb::archive(std::string path)
{
  DEBUG_LOG_MESSAGE("================== " << path << " ==========================")
  std::lock_guard<std::mutex> lk(_backup_mutex);
  if ( _conf.archive_path.empty() )
    return false;
  path += "/" + _name;

  COMMON_LOG_MESSAGE("Archive for '" << _name << " from " << _conf.backup_path << " ' to " << path)
  std::string error;
  if ( !copy_dir( _conf.backup_path, path, error ) )
  {
    DOMAIN_LOG_ERROR("Archive for '" << _name << "' fail. " << error );
    return true;
  }
  return false;
}


rocksdb_restore::rocksdb_restore(std::string name, const rocksdb_config conf, restore_db_type* rdb)
  : _name(name)
  , _conf(conf)
  , _rdb(rdb)
{
  
}
bool rocksdb_restore::restore() 
{
  COMMON_LOG_BEGIN("Restore for " << _name << " to " << _conf.path << " from " << _conf.restore_path )
  ::rocksdb::Status status = _rdb->RestoreDBFromLatestBackup( _conf.path, _conf.path, ::rocksdb::RestoreOptions() );
  COMMON_LOG_END("Restore for " << _name << " " << status.ToString() )
  if ( status.ok() )
    return true;
  
  std::vector< ::rocksdb::BackupInfo > info;
  _rdb->GetBackupInfo( &info );
  std::vector< ::rocksdb::BackupID > bads;
  _rdb->GetCorruptedBackups(&bads);
  if ( !bads.empty() )
  {
    std::stringstream ss;
    for (auto b : bads)
      ss << b;
    DOMAIN_LOG_ERROR("Есть поврежденные бэкапы " << ss.str());
  }

  for ( auto inf : info )
  {
    COMMON_LOG_BEGIN("Restore from backup_id=" << inf.backup_id )
    ::rocksdb::Status status = _rdb->RestoreDBFromBackup( inf.backup_id, _conf.path, _conf.path, ::rocksdb::RestoreOptions() );
    COMMON_LOG_END("Restore for " << _name << " " << status.ToString() )
    if ( status.ok() )
      return true;
  }
  return false;
}


}}
