#pragma once

#include <prefixdb/iprefixdb.hpp>
#include <wfc/iinterface.hpp>
#include <memory>

namespace wamba{ namespace prefixdb{ namespace service{

void prefixdb_cmd( std::shared_ptr<iprefixdb> db, wfc::iinterface::data_ptr d, wfc::iinterface::output_handler_t handler);

}}}
