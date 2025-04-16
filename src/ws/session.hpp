#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>

/**
 * Why enable_shared_from_this?
 * This is used to safely create a std::shared_ptr to an object from within that object itself.
 * It's useful in asynchronous systems where the object needs to keep itself alive untl a callback
 * completes.
 * 
 * This class implements asynchronous operations such as async_accept and async_read. This ensures
 * that the object that owns the callback isn't destroyed prematurely.
 */
class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
public:
    // Constructor accepts an rvalue socket (moved in, since sockets are non-copyable)
    explicit WebSocketSession(boost::asio::ip::tcp::socket&& socket);

    // Entry point: called right after socket is accepted
    void run();

private:
    // Called when a message is read from the socket
    void do_read();

    // Handles message received
    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);

    // Called after a message is written
    void on_write(boost::beast::error_code ec, std::size_t bytes_transferred);

    // WebSocket stream (wraps a TCP stream)
    boost::beast::websocket::stream<boost::beast::tcp_stream> ws_;

    // Internal buffer to hold received data
    boost::beast::flat_buffer buffer_;
};