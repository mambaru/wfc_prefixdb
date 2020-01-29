#pragma once

#include <string>
#include <vector>
#include <utility>

/** @brief wamba */
namespace wamba { 
  
  /** @brief prefixdb */
  namespace prefixdb {

    /**
    * @brief пара ключ-значение 
    */
    typedef std::pair<std::string, std::string> field_pair;

    /**
    * @brief список пар ключ-значение 
    */
    typedef std::vector< field_pair > field_list_t;

    /*! @brief список ключей */
    typedef std::vector< std::string > key_list_t;

  }
}
