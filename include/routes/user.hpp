#pragma once

#include <boost/asio/ip/tcp.hpp>

namespace handlers {
    void handle_session(boost::asio::ip::tcp::socket socket);
}