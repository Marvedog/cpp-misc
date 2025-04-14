#include "config.hpp"
#include "routes/health.hpp"
#include "routes/user.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <thread>

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;
namespace beast = boost::beast;


void route_request(http::request<http::string_body>& req, http::response<http::string_body>& res) {
    const std::string target = std::string(req.target());

    if (target.rfind("/v1/health", 0) == 0) {
        routes::handle_health(req, res);
    } else if (target.rfind("/v1/user", 0) == 0) {
        routes::handle_user_request(req, res);
    } else {
        res.result(http::status::not_found);
        res.set(http::field::content_type, "text/plain");
        res.body() = "404 Not Found";
        res.prepare_payload();
    }
}


void handle_session(tcp::socket socket) {
    try {
        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(socket, buffer, req);

        http::response<http::string_body> res;

        route_request(req, res);

        http::write(socket, res);

    } catch (const std::exception& e) {
        std::cerr << "Session error: " << e.what() << "\n";
    }
}


// ------------------------------------------------------------
// Main entry point: sets up the server and listens for clients.
// ------------------------------------------------------------
int main() {
    try {
        ConfigLoader config;
        config.load(".env");
    
        std::string db_host = config.get("DB_HOST");
        std::string port = config.get("PORT");

        std::cout << "Connecting to DB at: " << db_host << "\n";
        std::cout << "Server will start on port: " << port << "\n";

        boost::asio::io_context ioc;

        // Create a TCP acceptor listening on port 8080 (IPv4)
        tcp::acceptor acceptor(ioc, {tcp::v4(), 8080});

        std::cout << "Server is running at http://localhost:8080\n";

        // Infinite loop to handle incoming connections
        for (;;) {
            // Wait for and accept a new client connection
            tcp::socket socket = acceptor.accept();

            // Spawn a new thread to handle the client session
            std::thread([s = std::move(socket)]() mutable {
                handle_session(std::move(s));
            }).detach();  // Detach so the thread runs independently
        }
    } catch (std::exception& e) {
        // Print errors to stderr if anything goes wrong
        std::cerr << "Error: " << e.what() << "\n";
    }
}