#pragma once

#include <string>

namespace wamba{ namespace prefixdb{
  
struct restore_config
{
  // запретить востановление для данного конфига
  bool forbid = false;
  
  // последний бэкап
  // >0 - номер конкретного бэкапа
  // <0 - отсчет от последнего backup 
  int64_t backup_id = 0;
  
  // архив бэкапов
  std::string path  = "";
};


}}
