#include "session.hpp"
#include "websocket_session.hpp"

#include <boost/config.hpp>

#include <nlohmann/json.hpp>

#include <iostream>
#include <string>
#include <regex>


// Return a response for the given request.
//
// The concrete type of the response message (which depends on the
// request), is type-erased in message_generator.
template <class Body, class Allocator>
http::message_generator handle_request(beast::string_view doc_root, http::request<Body, http::basic_fields<Allocator>>&& req) {
    const std::string target = std::string(req.target());

    if (target.rfind("/v1/health", 0) == 0) {
        std::cout << "health route" << std::endl;

        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;
        // routes::handle_health(req, res);
    } else if (target.rfind("/v1/user", 0) == 0) {
        std::cout << "user route" << std::endl;

        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;
        // routes::handle_user(req, res, db_conn);
    } else if (req.target().starts_with("/chat")) {
        std::ifstream file("static/chat.html");
        if (file) {
            std::stringstream buffer;
            buffer << file.rdbuf();
    
            http::response<http::string_body> res;
            res.result(http::status::ok);
            res.set(http::field::content_type, "text/html");
            res.body() = buffer.str();
            res.prepare_payload();
            return res;
        } else {
            http::response<http::string_body> res{http::status::not_found, req.version()};
            res.result(http::status::not_found);
            res.set(http::field::content_type, "text/plain");
            res.body() = "Chat page not found.";
            res.prepare_payload();
            return res;
        }
    } else {
        http::response<http::string_body> res{http::status::not_found, req.version()};
        res.result(http::status::not_found);
        res.set(http::field::content_type, "text/plain");
        res.body() = "404 Not Found";
        res.prepare_payload();
        return res;
    }
}


// Constructor: wrap the raw TCP socket in a Beast WebSocket stream.
HttpSession::HttpSession(boost::asio::ip::tcp::socket&& socket, int room_id, const std::string& username, boost::shared_ptr<shared_state> const& state)
: 
stream_(beast::tcp_stream(std::move(socket))), 
room_id_(room_id), 
username_(username),
state_(state)  // Ensure all async ops are performed on the strand
{}

void HttpSession::run() {
    do_read();
}

void HttpSession::fail(beast::error_code ec, char const* what){
    // Don't report on canceled operations
    if(ec == net::error::operation_aborted)
        return;

    std::cerr << what << ": " << ec.message() << "\n";
}

/**
 * Starts an async read from the WebSocket stream.
 * This sets up a callback to `on_read` once a message is received.
 */
void HttpSession::do_read() {
    // Construct a new parser for each message
    parser_.emplace();

    // Apply a reasonable limit to the allowed size
    // of the body in bytes to prevent abuse.
    parser_->body_limit(10000);

    // Set the timeout.
    stream_.expires_after(std::chrono::seconds(30));

    // Read a request
    http::async_read(
        stream_,
        buffer_,
        parser_->get(),
        beast::bind_front_handler(
            &HttpSession::on_read,
            shared_from_this()));
}


void HttpSession::on_read(beast::error_code ec, std::size_t){
    // This means they closed the connection
    if(ec == http::error::end_of_stream)
    {
        stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
        return;
    }

    // Handle the error, if any
    if(ec)
        return fail(ec, "read");

    // See if it is a WebSocket Upgrade
    if(websocket::is_upgrade(parser_->get()))
    {
        // Create a websocket session, transferring ownership
        // of both the socket and the HTTP request.
        boost::make_shared<websocket_session>(
            stream_.release_socket(),
                state_)->run(parser_->release());
        return;
    }

    // Handle request
    http::message_generator msg =
        handle_request(state_->doc_root(), parser_->release());

    // Determine if we should close the connection
    bool keep_alive = msg.keep_alive();

    auto self = shared_from_this();

    // Send the response
    http::async_write(
        stream_, std::move(msg),
        [self, keep_alive](beast::error_code ec, std::size_t bytes)
        {
            self->on_write(ec, bytes, keep_alive);
        });
}


/**
 * Callback after a message is sent.
 * 
 * Clears the buffer and initiates another read (i.e., keep the connection alive).
 */
void HttpSession::on_write(beast::error_code ec, std::size_t, bool keep_alive){
    // Handle the error, if any
    if(ec)
        return fail(ec, "write");

    if(! keep_alive)
    {
        // This means we should close the connection, usually because
        // the response indicated the "Connection: close" semantic.
        stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
        return;
    }

    // Read another request
    do_read();
}