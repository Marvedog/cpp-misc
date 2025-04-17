#include "ws_chat.hpp"
#include <boost/beast/websocket.hpp>
#include <boost/beast/core.hpp>
#include <regex>
#include <iostream>

namespace websocket = boost::beast::websocket;
namespace beast = boost::beast;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

namespace routes {

void handle_websocket_chat(const std::string& target, tcp::socket socket) {
    std::smatch match;
    std::regex re("^/ws/chat/([a-zA-Z0-9_-]+)$");

    if (!std::regex_match(target, match, re)) {
        std::cerr << "Invalid chat route: " << target << "\n";
        return;
    }

    std::string room_name = match[1];
    std::cout << "New WebSocket connection for room: " << room_name << "\n";

    // Wrap the TCP socket in a WebSocket stream
    websocket::stream<tcp::socket> ws(std::move(socket));

    // Accept the WebSocket handshake
    ws.accept();

    // TODO: Add this connection to room registry & start listening to messages
    for (;;) {
        beast::flat_buffer buffer;
        ws.read(buffer);
        std::string msg = beast::buffers_to_string(buffer.data());

        std::cout << "[" << room_name << "] Received: " << msg << "\n";

        // Echo back for now
        ws.text(true);
        ws.write(asio::buffer("Echo: " + msg));
    }
}

}