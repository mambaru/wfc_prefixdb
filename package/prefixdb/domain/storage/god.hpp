#pragma once

#include "ifactory.hpp"
#include <wfc/asio.hpp>
#include <memory>

namespace wamba{ namespace prefixdb{ namespace god{

std::shared_ptr<ifactory> create(std::string type, ::wfc::asio::io_service& io);  

}}}
