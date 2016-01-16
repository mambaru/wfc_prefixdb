#pragma once

#include <prefixdb/iprefixdb.hpp>
#include <prefixdb/domain/storage/merge/merge.hpp>
#include <rocksdb/db.h>
#include <memory>

namespace rocksdb{ class DB;}

namespace wamba{ namespace prefixdb{
 
class rocksdb
  : public iprefixdb
{
  typedef ::rocksdb::DB db_type;
public:
  rocksdb( db_type* db, std::shared_ptr<iprefixdb> repli);
  virtual void set( request::set::ptr req, response::set::handler cb) override;
  virtual void get( request::get::ptr req, response::get::handler cb) override;
  virtual void has( request::has::ptr req, response::has::handler cb) override;
  virtual void del( request::del::ptr req, response::del::handler cb) override;
  virtual void inc( request::inc::ptr req, response::inc::handler cb) override;
  virtual void packed( request::packed::ptr req, response::packed::handler cb) override;
  virtual void range( request::range::ptr req, response::range::handler cb) override;
public:
  
  // void del_( request::del::ptr req, response::del::handler cb);

  template<merge_mode Mode, typename Res, typename ReqPtr, typename Callback>
  void merge_(ReqPtr req, Callback cb);

  
  template<typename Res, typename ReqPtr, typename Callback>
  void get_(ReqPtr req, Callback cb);

  template<typename Res, typename Batch, typename ReqPtr, typename Callback>
  void write_batch_(Batch& batch, ReqPtr req, Callback cb);
  
  std::unique_ptr<db_type> _db;
  std::shared_ptr<iprefixdb> _repli;
};

}}
