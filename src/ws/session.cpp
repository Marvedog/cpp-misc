#include "session.hpp"
#include "../chat_room_manager.hpp"
#include <iostream>

// Constructor: wrap the raw TCP socket in a Beast WebSocket stream.
WebSocketSession::WebSocketSession(boost::asio::ip::tcp::socket&& socket)
: ws_(boost::beast::tcp_stream(std::move(socket))) {}

/**
 * Starts the async WebSocket handshake.
 * 
 * 1. This is called after the TCP socket has been accepted.
 * 2. It asynchronously upgrades the connection from HTTP to WebSocket.
 * 3. Once complete, `on_accept` (in this case a lambda) is called.
 */
void WebSocketSession::run() {
    ws_.async_accept([self = shared_from_this()](boost::beast::error_code ec) {
        if (ec) {
            std::cerr << "WebSocket accept error: " << ec.message() << "\n";
            return;
        }

        // Join room after successful handshake
        ChatRoomManager::get_instance().join_room(self->room_id_, self);

        self->do_read();
    });
}

// Prepare the stream to send a text message (or binary)
void WebSocketSession::send(const std::string& msg) {
    ws_.text(true);
    ws_.async_write(boost::asio::buffer(msg),
        [self = shared_from_this()](boost::beast::error_code ec, std::size_t bytes) {
            self->on_write(ec, bytes);
        });
}

/**
 * Starts an async read from the WebSocket stream.
 * This sets up a callback to `on_read` once a message is received.
 */
void WebSocketSession::do_read() {
    ws_.async_read(buffer_, [self = shared_from_this()](boost::beast::error_code ec, std::size_t bytes_transferred) {
        self->on_read(ec, bytes_transferred);
    });
}


/**
 * Callback after a message is read.
 * 
 * 1. Converts the buffer into a string.
 * 2. Logs it to console.
 * 3. Echoes it back to the sender.
 * 
 * TODO: This is where we would persist the message to the db (?)
 */
void WebSocketSession::on_read(boost::beast::error_code ec, std::size_t) {
    // If the connection was closed gracefully
    if (ec == boost::beast::websocket::error::closed)
        return;

    if (ec) {
        std::cerr << "Read error: " << ec.message() << "\n";
        return;
    }

    std::string msg = boost::beast::buffers_to_string(buffer_.data());
    std::cout << "Received: " << msg << "\n";

    // Broadcast to room
    ChatRoomManager::get_instance().broadcast(room_id_, msg);
}


/**
 * Callback after a message is sent.
 * 
 * Clears the buffer and initiates another read (i.e., keep the connection alive).
 */
void WebSocketSession::on_write(boost::beast::error_code ec, std::size_t) {
    if (ec) {
        std::cerr << "Write error: " << ec.message() << "\n";
        return;
    }

    buffer_.consume(buffer_.size());  // Clear buffer
    do_read();  // Wait for next message
}