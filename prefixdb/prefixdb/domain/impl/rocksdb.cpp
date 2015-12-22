#include "rocksdb.hpp"
#include <wfc/logger.hpp>

namespace wamba{ namespace prefixdb {
  
rocksdb::rocksdb( db_type* db)
  : _db(db)
{}

void rocksdb::set( request::set::ptr req, response::set::handler cb)
{
  typedef response::set::field field_type;
  response::set::ptr res;
  if ( cb != nullptr ) res = std::make_unique<response::set>();

  ::rocksdb::WriteBatch batch;
  for ( const auto& field : req->fields)
  {
    batch.Put(field.key, field.val);
  }

  ::rocksdb::Status status = _db->Write( ::rocksdb::WriteOptions(), &batch);

  if ( status.ok() && res!=nullptr )
  {
    // Если нужен статус по каждому полю
    if ( !req->nores )
    {
      res->fields.reserve( req->fields.size() );
      for ( const auto& field : req->fields)
      {
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
  }

  if ( res!=nullptr )
    cb(std::move(res));
  
}

void rocksdb::get( request::get::ptr req, response::get::handler cb)
{
  typedef ::rocksdb::Slice slice_type;
  std::vector<slice_type> keys;
  keys.reserve( req->fields.size() );
  for ( const auto& field : req->fields)
  {
    keys.push_back(field);
  }

  std::vector<std::string> resvals;
  resvals.reserve(keys.size());
  std::vector< ::rocksdb::Status> status
    = _db->MultiGet( ::rocksdb::ReadOptions(), keys, &resvals);

  if ( keys.size() != resvals.size() )
  {
    DOMAIN_LOG_FATAL("rocksdb::get keys.size() != resvals.size() " << keys.size() << "!=" << resvals.size())
    abort();
  }

  auto res = std::make_unique<response::get>();
  res->status = common_status::OK;
  for ( size_t i = 0; i!=resvals.size(); ++i)
  {
    if ( status[i] == ::rocksdb::Status::OK() )
    {
      // TODO
    }
    else if ( res->status != common_status::OK)
    {
      res->status = common_status::SomeFieldFail;
    }
    else
    {
      // задать статус
    }
  }
}

void rocksdb::has( request::has::ptr req, response::has::handler cb)
{
  DOMAIN_LOG_FATAL("rocksdb::has not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}


void rocksdb::del( request::del::ptr req, response::del::handler cb) 
{

  DOMAIN_LOG_FATAL("rocksdb::del not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

void rocksdb::inc( request::inc::ptr req, response::inc::handler cb) 
{
  DOMAIN_LOG_FATAL("rocksdb::inc not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

void rocksdb::upd( request::upd::ptr req, response::upd::handler cb) 
{
  DOMAIN_LOG_FATAL("rocksdb::upd not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

}}
