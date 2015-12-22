#pragma once

#include "ifactory.hpp"
#include <memory>

namespace wamba{ namespace prefixdb{ namespace god{

std::shared_ptr<ifactory> create(std::string type);  

}}}
