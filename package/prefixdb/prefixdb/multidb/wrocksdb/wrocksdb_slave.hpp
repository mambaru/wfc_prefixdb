#pragma once

#include <prefixdb/prefixdb/multidb/options/db_config.hpp>
#include <mutex>
namespace rocksdb{ class DBWithTTL;}

namespace wamba{ namespace prefixdb{
  
class since_reader;
  
class wrocksdb_slave
  : public std::enable_shared_from_this<wrocksdb_slave>
{
  typedef wfc::workflow::timer_id_t timer_id_t;

public:
  typedef ::rocksdb::DBWithTTL db_type;
  
  wrocksdb_slave(std::string name, std::string path, const slave_config& opt, db_type& db);
  
  void start();
  
  void stop();
  
private:
  void initial_load_();
  void query_initial_range_(size_t snapshot, const std::string& from, bool beg);
  
  void start_();
  
  void create_updates_requester_();
  
  static request::get_updates_since::ptr updates_generator_(
    std::weak_ptr<wrocksdb_slave> wthis,
    response::get_updates_since::ptr, 
    std::shared_ptr<request::get_updates_since> preq
  );
  
  void logs_parser_( response::get_updates_since::ptr& res);
  void create_diff_timer_();
  void create_seq_timer_();
  void     write_sequence_number_(uint64_t seq);
  uint64_t read_sequence_number_();
  
  
private:
  std::string _name;
  std::string _path;
  slave_config _opt;
  db_type& _db;
  std::shared_ptr<since_reader> _log_parser;
  
  std::atomic<time_t> _last_update_time;
  std::atomic<size_t> _update_counter;
  std::atomic<std::ptrdiff_t> _current_differens;
  std::atomic<size_t> _last_sequence;
  std::atomic<size_t> _lost_counter;
  
  timer_id_t _slave_timer_id = -1;
  timer_id_t _diff_timer_id = -1;
  timer_id_t _seq_timer_id = -1;
  
  std::shared_ptr< ::wfc::workflow> _workflow;
  typedef std::mutex mutex_type;
  mutex_type _mutex;
  bool is_started = false;
};
 

}}
