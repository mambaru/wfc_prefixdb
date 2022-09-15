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

    typedef wjson::object<
      set,
      wjson::member_list<
        wjson::member<n_prefix, set, std::string, &set::prefix>,
        wjson::member<n_sync,   set, bool, &set::sync>,
        wjson::member<n_nores,  set, bool, &set::nores>,
        wjson::member<n_noval,  set, bool, &set::noval>,
        wjson::member<n_snapshot,  set, size_t, &set::snapshot>,
        wjson::member<n_fields, set, field_list_t, &set::fields, fields_list_json >
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
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

    typedef wjson::object<
      set,
      wjson::member_list<
        wjson::member<n_prefix, set, std::string, &set::prefix>,
        wjson::member<n_status, set, common_status, &set::status, common_status_json>,
        wjson::member<n_fields, set, field_list_t, &set::fields, fields_list_json>
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

}}
