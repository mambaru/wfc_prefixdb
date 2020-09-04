#pragma once

#include "ifactory.hpp"
#include <wfc/asio.hpp>
#include <memory>

namespace wamba{ namespace prefixdb{

class god
{
public:
  static std::shared_ptr<ifactory> create(const std::string& type, boost::asio::io_context& io);
};

}}
