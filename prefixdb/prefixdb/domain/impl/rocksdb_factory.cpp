
#include "rocksdb_factory.hpp"
#include <rocksdb/db.h>
#include <rocksdb/env.h>
#include <rocksdb/options.h>

#include <memory>
#include <iostream>
#include <sys/stat.h>
#include <wfc/logger.hpp>
#include <list>
#include "prefixdb_t.hpp"

#include <wdb/ldb/rocksdb/storage.hpp>

namespace wamba{ namespace prefixdb{

struct rocksdb_factory::context
{
  typedef ::wdb::ldb::rocksdb::storage<std::string, std::string> storage_type;
  typedef prefixdb_t<storage_type> prefixdb_type;
  typedef std::shared_ptr<prefixdb_type> prefixdb_ptr;
  
  storage_type::env_type* env;
  storage_type::options_type options;
  std::string path;
  
};

rocksdb_factory::~rocksdb_factory()
{
  _context->env = nullptr;
}

void rocksdb_factory::initialize(std::string db_path, std::string ini_path) 
{
  std::lock_guard<std::mutex> lk(_mutex);
  typedef rocksdb_factory::context::storage_type storage_type;
  typedef storage_type::options_type options_type;
  _context = std::make_shared<rocksdb_factory::context>();
  _context->env = ::rocksdb::Env::Default();
  _context->path = db_path;
  //_context->storage = std::make_shared< storage_type >();
  
  auto status = options_type::load(ini_path, _context->env, _context->options  );
  //auto status = ::rocksdb::LoadOptionsFromFile(ini_path, _context->env, &(_context->options), &(_context->cdf) );
  
  if ( !status.ok() )
  {
    DOMAIN_LOG_FATAL("rocksdb_factory::initialize: " << status.ToString());
    abort();
  }
  
}

ifactory::prefixdb_ptr rocksdb_factory::create(std::string prefix, bool create_if_missing) 
{
  std::lock_guard<std::mutex> lk(_mutex);
  typedef rocksdb_factory::context::prefixdb_type prefixdb_type;
  typedef rocksdb_factory::context::storage_type storage_type;
  auto stg = std::make_shared<storage_type>();
  auto prf = std::make_shared<prefixdb_type>(stg, nullptr);
  std::string path = _context->path + "/" + prefix;
  auto opt = _context->options;
  opt.path = _context->path + "/" + prefix;
  opt.create_if_missing = create_if_missing;
  opt.env = _context->env;
  stg->open(opt);
  return prf;
}

}}
