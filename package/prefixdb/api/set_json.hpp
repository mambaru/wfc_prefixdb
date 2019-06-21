#pragma once

#include <prefixdb/api/set.hpp>
#include <prefixdb/api/fields_json.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  /**
   *  @brief set json
      @code{.json}
      {
        "prefix":"префикс",
        "nores":true,
        "noval":false,
        "sync" :false,
        "fields":
          {
            "ключ1":<<json>>,
            "ключ2":<<json>>,
            "ключ3":<<json>>
          }
      }
      @endcode
      @see request::set
   */
  struct set_json
  {
    JSON_NAME(prefix)
    JSON_NAME(nores)
    JSON_NAME(noval)
    JSON_NAME(snapshot)
    JSON_NAME(fields)
    JSON_NAME(sync)

    typedef wfc::json::object<
      set,
      wfc::json::member_list<
        wfc::json::member<n_prefix, set, std::string, &set::prefix>,
        wfc::json::member<n_sync,   set, bool, &set::sync>,
        wfc::json::member<n_nores,  set, bool, &set::nores>,
        wfc::json::member<n_noval,  set, bool, &set::noval>,
        wfc::json::member<n_snapshot,  set, size_t, &set::snapshot>,
        wfc::json::member<n_fields, set, field_list_t, &set::fields, fields_list_json >
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

namespace response
{
  /**
    * @brief set json
      @code{.json}
      {
        "prefix":"префикс",
        "status":"OK",
        "fields":{
          "ключ1":<<json>>,
          "ключ2":<<json>>,
          "ключ3":<<json>>
        }
      }
      @endcode
  */

  struct set_json
  {
    JSON_NAME(prefix)
    JSON_NAME(status)
    JSON_NAME(fields)

    typedef wfc::json::object<
      set,
      wfc::json::member_list<
        wfc::json::member<n_prefix, set, std::string, &set::prefix>,
        wfc::json::member<n_status, set, common_status, &set::status, common_status_json>,
        wfc::json::member<n_fields, set, field_list_t, &set::fields, fields_list_json>
      >
    > type;
    typedef type::target target;
    typedef type::serializer serializer;
    typedef type::member_list member_list;
  };
}

}}
