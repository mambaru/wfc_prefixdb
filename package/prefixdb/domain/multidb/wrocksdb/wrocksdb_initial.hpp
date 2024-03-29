#pragma once

#include <prefixdb/domain/multidb/options/db_config.hpp>
#include <wflow/owner.hpp>
#include <mutex>

namespace rocksdb{ class DBWithTTL;}

namespace wamba{ namespace prefixdb{

class since_reader;

class wrocksdb_initial
  : public std::enable_shared_from_this<wrocksdb_initial>
{
  typedef wflow::workflow::timer_id_t timer_id_t;

public:
  typedef ::rocksdb::DBWithTTL db_type;

  wrocksdb_initial(std::string name, const initial_config& opt, db_type& db);

  void load(std::function<void(size_t)> ready);

  void stop();

private:

  void query_initial_range_(size_t snapshot, const std::string& from, bool beg, std::function<void()> ready);

private:
  std::string _name;
  initial_config _opt;
  db_type& _db;

  std::shared_ptr<wflow::workflow> _workflow;
  typedef std::mutex mutex_type;
  wflow::owner _owner;
  std::atomic<time_t> _log_timer;
};

}}
