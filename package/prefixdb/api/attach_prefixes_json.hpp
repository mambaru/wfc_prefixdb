#pragma once

#include <prefixdb/api/attach_prefixes.hpp>
#include <prefixdb/api/common_status_json.hpp>
#include <wfc/json.hpp>

namespace wamba { namespace prefixdb {

namespace request 
{
  struct attach_prefixes_json
  {
    JSON_NAME(prefixes)
    JSON_NAME(opendb)
    
    typedef wjson::object<
      attach_prefixes,
      wjson::member_list<
        wjson::member<n_prefixes, attach_prefixes, attach_prefixes::prefix_list, &attach_prefixes::prefixes, wjson::vector_of_strings<50> >,
        wjson::member<n_opendb, attach_prefixes, bool, &attach_prefixes::opendb >
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

namespace response
{
  struct attach_prefixes_json
  {
    JSON_NAME(status)

    typedef wjson::object<
      attach_prefixes,
      wjson::member_list<
        wjson::member<n_status, attach_prefixes, common_status, &attach_prefixes::status, common_status_json>
      >
    > meta;
    typedef meta::target target;
    typedef meta::serializer serializer;
    typedef meta::member_list member_list;
  };
}

}}
