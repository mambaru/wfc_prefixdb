#pragma once

#include <string>
#include <prefixdb/prefixdb/multidb/options/db_config.hpp>

namespace wamba{ namespace prefixdb{

struct multidb_config: db_config
{
  // максимальное количество ключей на запрос
  size_t keys_per_req = 0;
  // ограничкение на размер ключа
  size_t key_size_limit = 0;
  // ограничкение на размер значений
  size_t value_size_limit = 0;
  // ограничкение на размер префикса
  size_t prefix_size_limit = 0;
  // ограничкение на общее число префиксов
  size_t max_prefixes = 0;
  // Предварительное открытие всех баз префиксов
  bool preopen = true;
};

}}
