#include "rocksdb.hpp"
#include <wfc/logger.hpp>
#include <rocksdb/db.h>
#include <rocksdb/write_batch.h>

namespace wamba{ namespace prefixdb {
  
rocksdb::rocksdb( db_type* db, std::shared_ptr<iprefixdb> repli)
  : _db(db)
  , _repli(repli)
{}

// Гарантированно req!=nullptr, may be cb==nullptr
void rocksdb::set( request::set::ptr req, response::set::handler cb)
{
  typedef response::set::field field_type;
  ::rocksdb::WriteBatch batch;
  
  for ( const auto& field : req->fields)
  {
    batch.Put(field.key, field.val);
  }

  ::rocksdb::Status status = _db->Write( ::rocksdb::WriteOptions(), &batch);
  
  if ( cb == nullptr )
    return;

  // **********************************************************
  // формируем ответ
  auto res = std::make_unique<response::set>();
  res->prefix = std::move(req->prefix);

  if ( status.ok() )
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
    COMMON_LOG_ERROR("rocksdb::set WriteError: " << status.ToString() )
  }
  cb(std::move(res));
}

namespace {

inline void get_mov_val(response::has::field&,     std::string&    ) { /* рекламное место здается */ }
inline void get_mov_val(response::get::field& fld, std::string& val) {    fld.val = std::move(val);  }
inline void get_mov_val(response::del::field& fld, std::string& val) {    fld.val = std::move(val);  }
inline void get_mov_val(response::inc::field& fld, std::string& val) {    fld.val = std::move(val);  }

}

template<typename Res, typename ReqPtr, typename Callback>
void rocksdb::get_(ReqPtr req, Callback cb)
{
  typedef Res response_type;
  typedef ::rocksdb::Slice slice_type;
  
  std::vector<slice_type> keys;
  keys.reserve(req->fields.size() );
  for ( const auto& fld: req->fields ) keys.push_back(fld.key);

  std::vector<std::string> resvals;
  resvals.reserve(keys.size());
  std::vector< ::rocksdb::Status> status
    = _db->MultiGet( ::rocksdb::ReadOptions(), keys, &resvals);

  if ( keys.size() != resvals.size() )
  {
    DOMAIN_LOG_FATAL("rocksdb::get keys.size() != resvals.size() " << keys.size() << "!=" << resvals.size())
    abort();
  }

  // **********************************************************
  // формируем ответ
  
  bool ok = true;
  auto res = std::make_unique<response_type>();
  res->prefix = std::move(req->prefix);
  res->status = common_status::OK;
  res->fields.reserve(resvals.size());
  for ( size_t i = 0; i!=resvals.size(); ++i)
  {
    typename response_type::field fld;
    fld.key = std::move( req->fields[i].key );
    get_mov_val(fld, resvals[i]);

    fld.type = status[i].ok()
               ? field_type::any
               : field_type::none;
    res->fields.push_back( std::move(fld) );
    if ( ok ) ok = status[i].ok();
  }
  if (!ok) res->status = common_status::SomeFieldFail;
  cb( std::move(res) );
  
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
  for ( const auto& field : req->fields)
  {
    batch.Delete(field.key);
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
    bool noval = req->noval;
    this->get_<response::del>( std::move(req), [this, &batch, &cb, noval](response::del::ptr res)
    {
      ::rocksdb::Status status = this->_db->Write( ::rocksdb::WriteOptions(), &batch);
      res->status = status.ok() ? common_status::OK : common_status::WriteError;
      
      // Очищаем, если значения не нужны. Один хрен они были запрошены и перемещены. 
      // Имеет смысл только на больших строчках.
      if ( noval ) for (auto& fld : res->fields) fld.val.clear();

      cb( std::move(res) );
    });
  }
}

void rocksdb::inc( request::inc::ptr req, response::inc::handler cb) 
{
  // TODO: использовать Merge
  
  // _db->Merge();
  
  this->get_<response::inc>( std::move(req), [this, &cb]( response::inc::ptr res)
  {
    //for ( auto& fld: )
    cb( std::move(res) );
  } );
  
}

void rocksdb::upd( request::upd::ptr req, response::upd::handler cb) 
{
  DOMAIN_LOG_FATAL("rocksdb::upd not IMPL " << (req!=nullptr) << " " << (cb!=nullptr))
  abort();
}

}}
