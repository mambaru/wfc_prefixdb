#include <wfc/memory.hpp>
#include <prefixdb/api/common_status.hpp>

namespace wamba{ namespace prefixdb { namespace{
  
template<common_status Status, typename Res, typename ReqPtr, typename Callback>
inline bool send_error(const ReqPtr& req, const Callback& cb)
{
  if ( cb!=nullptr )
  {
    auto res = std::make_unique<Res>();
    res->prefix = std::move(req->prefix);
    res->status = Status;
    cb( std::move(res) );
    return true;
  }
  return false;
}

template<typename Res, typename ReqPtr, typename Callback>
inline bool prefix_not_found(const ReqPtr& req, const Callback& cb)
{
  return send_error<common_status::PrefixNotFound, Res>(std::move(req), std::move(cb) );
}

template<typename Res, typename ReqPtr, typename Callback>
inline bool create_prefix_fail(const ReqPtr& req, const Callback& cb)
{
  return send_error<common_status::CreatePrefixFail, Res>(std::move(req), std::move(cb) );
}

template<typename Res, typename ReqPtr, typename Callback>
inline bool empty_fields(const ReqPtr& req, const Callback& cb)
{
  if ( req->fields.empty() )
  {
    send_error<common_status::EmptyFields, Res>(std::move(req), std::move(cb) );
    return true;
  }
  return false;
}

    
}}}
