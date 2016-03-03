#pragma once

#include <string>
#include <prefixdb/domain/storage/rocksdb_config.hpp>

namespace wamba{ namespace prefixdb{
  
struct multidb_config: rocksdb_config
{
  // максимальное количество ключей на запрос
  size_t keys_per_req = 100;
  // ограничкение на размер ключа
  size_t key_size_limit = 128;
  // ограничкение на размер значений
  size_t value_size_limit = 1024*10;
  // ограничкение на размер префикса
  size_t prefix_size_limit = 256;
  // ограничкение на общее число префиксов
  size_t max_prefixes = 128;
  // Предварительное открытие всех баз префиксов
  bool preopen = true;
  
};

}}
