#pragma once

#include <prefixdb/prefixdb/multidb/options/db_config.hpp>
#include <string>
#include <vector>

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
  
  // Разрешены только эти префиксы (пустой значит все)
  std::vector<std::string> allowed_prefixes;
  // Список запрещенных префиксов 
  std::vector<std::string> denied_prefixes;

};

}}
