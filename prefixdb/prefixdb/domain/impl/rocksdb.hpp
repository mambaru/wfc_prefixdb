#pragma once

#include <prefixdb/iprefixdb.hpp>
#include <rocksdb/db.h>
#include <memory>

namespace wamba{ namespace prefixdb{
 
class rocksdb
  : public iprefixdb
{
  typedef ::rocksdb::DB db_type;
public:
  rocksdb( db_type* db);
  virtual void set( request::set::ptr req, response::set::handler cb) override;
  virtual void get( request::get::ptr req, response::get::handler cb) override;
  virtual void has( request::has::ptr req, response::has::handler cb) override;
  virtual void del( request::del::ptr req, response::del::handler cb) override;
  virtual void inc( request::inc::ptr req, response::inc::handler cb) override;
  virtual void upd( request::upd::ptr req, response::upd::handler cb) override;
public:
  std::unique_ptr<db_type> _db;

};

}}
