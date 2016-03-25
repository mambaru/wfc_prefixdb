#pragma once

#include <prefixdb/domain/storage/options/db_config.hpp>

namespace rocksdb{ class BackupableDB;}

namespace wamba{ namespace prefixdb{
  
class since_reader;
  
class wrocksdb_slave
  : public std::enable_shared_from_this<wrocksdb_slave>
{
  typedef wfc::workflow::timer_id_t timer_id_t;

public:
  typedef ::rocksdb::BackupableDB db_type;
  
  wrocksdb_slave(std::string name, const slave_config opt, db_type& db);
  
  void start();
  
  void stop();
  
private:
  void create_updates_requester_();
  request::get_updates_since::ptr updates_handler_(response::get_updates_since::ptr, std::shared_ptr<request::get_updates_since> preq);
  void logs_parser_( response::get_updates_since::ptr& res);
  void create_diff_timer_();
  void create_seq_timer_();
private:
  std::string _name;
  slave_config _opt;
  db_type& _db;
  std::shared_ptr<since_reader> _log_parser;
  
  std::atomic<time_t> _last_update_time;
  std::atomic<size_t> _update_counter;
  std::atomic<size_t> _current_differens;
  std::atomic<size_t> _last_sequence;
  
  timer_id_t _slave_timer_id = -1;
  timer_id_t _diff_timer_id = -1;
  timer_id_t _seq_timer_id = -1;
};
 

}}
