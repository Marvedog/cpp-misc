#pragma once

#include "beast.hpp"
#include "net.hpp"
#include "shared_state.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/optional.hpp>
#include <boost/smart_ptr.hpp>

#include <string>
#include <iostream>

/**
 * Why enable_shared_from_this?
 * This is used to safely create a std::shared_ptr to an object from within that object itself.
 * It's useful in asynchronous systems where the object needs to keep itself alive untl a callback
 * completes.
 * 
 * This class implements asynchronous operations such as async_accept and async_read. This ensures
 * that the object that owns the callback isn't destroyed prematurely.
 */
class HttpSession : public boost::enable_shared_from_this<HttpSession> {
public:
    // Constructor accepts an rvalue socket (moved in, since sockets are non-copyable)
    // HttpSession(boost::asio::ip::tcp::socket&& socket, int room_id, const std::string& username, boost::shared_ptr<shared_state> const& state);
    HttpSession(boost::asio::ip::tcp::socket&& socket, boost::shared_ptr<shared_state> const& state);

    // Entry point: called right after socket is accepted
    void run();

private:
    void fail(beast::error_code ec, char const* what);

    // Called when a message is read from the socket
    void do_read();

    // Handles message received
    void on_read(beast::error_code ec, std::size_t);

    // Called after a message is written
    void on_write(beast::error_code ec, std::size_t, bool close);

    // WebSocket stream (wraps a TCP stream)
    beast::tcp_stream stream_;

    // Internal buffer to hold received data
    beast::flat_buffer buffer_;

    // TODO: What is this for
    boost::shared_ptr<shared_state> state_;

    // TODO: WHat is this for
    struct send_lambda;

    // The parser is stored in an optional container so we can
    // construct it from scratch it at the beginning of each new message.
    boost::optional<http::request_parser<http::string_body>> parser_;

    // Default room id for now
    int room_id_ = 1;

    // Username 
    std::string username_ = "Joe";
};