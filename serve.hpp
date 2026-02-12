#pragma once

#include <boost/asio.hpp>
#include "types.hpp"

void server(boost::asio::io_context& io_context, unsigned short port, IPC_globals & ipc);

