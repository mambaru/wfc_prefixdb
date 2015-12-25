#pragma once

#include <prefixdb/iprefixdb.hpp>
#include <memory>

namespace wamba{ namespace prefixdb{

  
template<typename Storage>
class prefixdb_t
  : public iprefixdb
{
  typedef Storage db_type;
  typedef std::shared_ptr<db_type> db_ptr;
  typedef std::shared_ptr<iprefixdb> prefixdb_ptr;
public:
  prefixdb_t( db_ptr db, prefixdb_ptr repli)
    : _db(db)
    , _repli(repli)
  {
  }
  
  virtual void set( request::set::ptr req, response::set::handler cb) override
  {
    for ( const auto& field : req->fields)
    {
      _db->insert( std::make_pair(field.key, field.val) );
    }
    
    if ( cb == nullptr )
      return;

    // **********************************************************
    // формируем ответ
    auto res = std::make_unique<response::set>();
    res->prefix = std::move(req->prefix);

    if ( /*status.ok()*/true )
    {
      // Если нужен статус по каждому полю
      if ( !req->nores )
      {
        res->fields.reserve( req->fields.size() );
        for ( const auto& field : req->fields)
        {
          typedef response::set::field field_type;
          
          field_type rf;
          rf.key = std::move( field.key );
          // если нужно записанное значение в ответе
          if ( !req->noval )
          {
            rf.val = std::move( field.val );
          }
          res->fields.push_back( std::move(rf) );
        }
      }
      res->status = common_status::OK;
    }
    else if (res!=nullptr)
    {
      res->status = common_status::WriteError;
      COMMON_LOG_ERROR("rocksdb::set WriteError: " << "status.ToString()" )
    }

    cb(std::move(res));


    
  }
  
  virtual void get( request::get::ptr req, response::get::handler cb) override
  {
    
  }
  
  virtual void has( request::has::ptr req, response::has::handler cb) override
  {
    
  }
  
  virtual void del( request::del::ptr req, response::del::handler cb) override
  {
    
  }
  
  virtual void inc( request::inc::ptr req, response::inc::handler cb) override
  {
    
  }
  
  virtual void upd( request::upd::ptr req, response::upd::handler cb) override
  {
    
  }
  
public:
  
  // void del_( request::del::ptr req, response::del::handler cb);
  
  template<typename Res, typename ReqPtr, typename Callback>
  void get_(ReqPtr req, Callback cb);

  db_ptr _db;
  std::shared_ptr<iprefixdb> _repli;
};

}}
