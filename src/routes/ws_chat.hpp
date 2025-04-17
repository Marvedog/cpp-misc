#pragma once

#include <boost/beast/websocket.hpp>
#include <boost/beast/core.hpp>
#include <boost/asio.hpp>
#include <string>

namespace routes {

void handle_websocket_chat(const std::string& target, boost::asio::ip::tcp::socket socket);

}