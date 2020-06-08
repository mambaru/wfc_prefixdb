#include <wfc/json.hpp>
#include <iow/io/types.hpp>
#include <prefixdb/api/common_status.hpp>
#include <prefixdb/logger.hpp>

namespace wamba{ namespace prefixdb { namespace aux{


inline std::string& get_key(std::string& key)
{
  return key;
}

inline std::string& get_key( std::pair<std::string, std::string>& field)
{
  return field.first;
}

template<typename Json, typename Res, typename ReqPtr, typename Callback>
bool check_params(ReqPtr& req, Callback& cb)
{
  typedef Json params_json;
  typedef typename params_json::target params_t;
  typedef typename params_json::serializer serializer;
  bool noerr = true;
  params_t pkg;

  ::wfc::json::json_error e;
  for (const auto& field : req->fields )
  {
    serializer()(pkg, field.second.begin(), field.second.end(), &e );
    if ( e )
    {
      PREFIXDB_LOG_ERROR( "JSONRPC params error: " << wfc::json::strerror::message_trace( e, field.second.begin(), field.second.end() ) );
      noerr=false;
      break;
    }
  }

  if ( !noerr && cb!=nullptr)
  {
    auto res = std::make_unique<Res>();
    res->status = common_status::InvalidFieldValue;
    cb( std::move(res) );
  }
  return noerr;
}

template<typename Res, typename ReqPtr>
std::unique_ptr<Res> create_result_ok(ReqPtr& req)
{
  auto res = std::make_unique<Res>();
  res->prefix = std::move(req->prefix);
  res->status = common_status::OK ;
  return std::move(res);
}

template<typename Res, typename ReqPtr>
std::unique_ptr<Res> create_io_error(ReqPtr& req)
{
  auto res = std::make_unique<Res>();
  res->prefix = std::move(req->prefix);
  res->status = common_status::IOError ;
  return std::move(res);
}

inline std::string repair_json_value(const std::string& prefix, const std::string& key, std::string&& value, bool* fix)
{
  auto beg = value.begin();
  auto end = value.end();
  wjson::json_error er;
  // если нет ошибок и нет мусора в конце
  auto last = wjson::parser::parse_value(beg, end, &er);
  if ( !er && last==end )
    return value;

  if ( fix!=nullptr )
      *fix = true;

  // Нет ошибок, но мусор в конце. Возвращаем
  if ( !er )
  {
    PREFIXDB_LOG_WARNING("Обнаружен мусор в префиксе '" << prefix <<"' для ключа '" << key << "':")
    PREFIXDB_LOG_WARNING("\t корректная часть: '" << std::string(beg, last) << "'" )
    PREFIXDB_LOG_WARNING("\t мусор: '" << std::string(last, end) << "'" )
    return std::string(beg, last);
  }

  if ( er )
  {
    er.reset();
    last = wjson::parser::parse_integer(beg, end, &er);
    if ( !er )
    {
      PREFIXDB_LOG_WARNING("Обнаружен мусор в префиксе '" << prefix <<"' для ключа '" << key << "':")
      PREFIXDB_LOG_WARNING("\t корректная часть (предположительно int): '" << std::string(beg, last) << "'" )
      PREFIXDB_LOG_WARNING("\t мусор: '" << std::string(last, end) << "'" )
      return std::string(beg, last);
    }
  }

  std::string trash_json;
  trash_json.reserve( value.size() );
  wjson::value<std::string>::serializer()(value, std::back_inserter(trash_json));

  PREFIXDB_LOG_WARNING("Обнаружен мусор в префиксе '" << prefix <<"' для ключа '" << key << "':")
  PREFIXDB_LOG_WARNING("\t значение было сериализовано в строку: '" << trash_json << "'" )
  PREFIXDB_LOG_WARNING("\t старое значение: '" << value << "'" )
  return trash_json;
}

}}}
