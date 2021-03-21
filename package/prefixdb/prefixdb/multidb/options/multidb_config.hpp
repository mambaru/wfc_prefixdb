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
  
  // Разрешены только эти префиксы на запись (пустой значит все)
  std::vector<std::string> writable_prefixes;
  // Список префиксов доступных только для чтения
  std::vector<std::string> readonly_prefixes;
  
  // Slave забирает только доступные для записи
  // Если префикс запрещен, то база префикса работает, просто блокируются все запросы к ней
  // При работе в режиме slave запрещенные префиксы не реплицируються
  // На мастере они поддерживаються и выдаються в get_all_prefixes, но при запросе к ним будет ошибка 

};

}}
