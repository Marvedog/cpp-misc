#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include "handlers/user.hpp"


using tcp = boost::asio::ip::tcp;


// ------------------------------------------------------------
// Main entry point: sets up the server and listens for clients.
// ------------------------------------------------------------
int main() {
    try {
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
                handlers::handle_session(std::move(s));
            }).detach();  // Detach so the thread runs independently
        }
    } catch (std::exception& e) {
        // Print errors to stderr if anything goes wrong
        std::cerr << "Error: " << e.what() << "\n";
    }
}