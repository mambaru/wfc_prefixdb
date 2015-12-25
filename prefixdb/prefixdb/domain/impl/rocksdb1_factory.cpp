
#include "rocksdb1_factory.hpp"
#include <rocksdb/db.h>
#include <rocksdb/env.h>
#include <rocksdb/options.h>

#include <memory>
#include <iostream>
#include <sys/stat.h>
#include <wfc/logger.hpp>
#include <list>


/*
 #include <leveldb/db.h>
 #include <leveldb/env.h>
 #include <leveldb/filter_policy.h>
 #include <leveldb/cache.h>
 #include <leveldb/write_batch.h>
*/


#include "rocksdb1.hpp"



namespace rocksdb
{

Status LoadOptionsFromFile(const std::string& options_file_name, Env* env,
                           DBOptions* db_options,
                           std::vector<ColumnFamilyDescriptor>* cf_descs);

}


namespace wamba{ namespace prefixdb{

struct rocksdb1_factory::context
{
  typedef ::rocksdb::ColumnFamilyDescriptor CFD;
  typedef std::vector<CFD> CFD_list;
  ::rocksdb::Env* env;
  ::rocksdb::Options options;
  CFD_list cdf;
  std::string path;
};

rocksdb1_factory::~rocksdb1_factory()
{
  _context->env = nullptr;
}

void rocksdb1_factory::initialize(std::string db_path, std::string ini_path) 
{
  std::lock_guard<std::mutex> lk(_mutex);
  
  _context = std::make_shared<rocksdb1_factory::context>();
  _context->env = ::rocksdb::Env::Default();
  _context->path = db_path;
  
  
  auto status = ::rocksdb::LoadOptionsFromFile(ini_path, _context->env, &(_context->options), &(_context->cdf) );
  if ( !status.ok() )
  {
    DOMAIN_LOG_FATAL("rocksdb1_factory::initialize: " << status.ToString());
    abort();
  }
  
}

ifactory::prefixdb_ptr rocksdb1_factory::create(std::string prefix, bool create_if_missing) 
{
  std::lock_guard<std::mutex> lk(_mutex);
  _context->options.env = _context->env;
  _context->options.create_if_missing = create_if_missing;
  //_context->options.OptimizeForPointLookup();
  //_context->options.OptimizeLevelStyleCompaction();
  //_context->options.OptimizeUniversalStyleCompaction();
  //_context->options.max_open_files = 1024 * 100;
  //_context->options.filter_policy = ::rocksdb::NewBloomFilterPolicy(20);
  //_context->options.write_buffer_size = 512 * 1024 * 1024;
  //_context->options.block_cache = ::rocksdb::NewLRUCache( conf.cache_size * 1024 * 1024);

  
  std::string path = _context->path + "/" + prefix;
  ::rocksdb::DB* db;
  
  auto status =  ::rocksdb::DB::Open( _context->options, path, &db);
  if ( status.ok() )
  {
    return std::make_shared<rocksdb>(db, nullptr);
  }

  DOMAIN_LOG_FATAL("rocksdb1_factory::create: " << status.ToString());
  return nullptr;
}

}}
