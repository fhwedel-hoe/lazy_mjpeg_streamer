#pragma once

#include <boost/asio.hpp>
#include "types.hpp"

void server(boost::asio::io_service& io_service, unsigned short port, IPC_globals & ipc);

