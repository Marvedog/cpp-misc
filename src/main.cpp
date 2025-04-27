#include "config.hpp"
#include "routes/health.hpp"
#include "routes/user.hpp"
#include "ws/session.hpp"
#include "ws/listener.hpp"
#include "ws/shared_state.hpp"

#include <pqxx/pqxx>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <thread>

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;
namespace websocket = boost::beast::websocket;
namespace beast = boost::beast;

using Request = http::request<http::string_body>;
using Response = http::response<http::string_body>;


/*
void handle_session(tcp::socket socket, std::shared_ptr<pqxx::connection> db_conn, boost::asio::io_context& ioc) {
    try {
        beast::flat_buffer buffer;
        Request req;
        http::read(socket, buffer, req);


        const std::string target = std::string(req.target());
        std::map<std::string, std::string> query_params;

        std::size_t query_pos = target.find("?");
        if (query_pos != std::string::npos) {
            std::string query = target.substr(query_pos + 1);
            std::stringstream ss(query);
            std::string pair;
            while (std::getline(ss, pair, '&')) {
                std::size_t eq_pos = pair.find('=');
                if (eq_pos != std::string::npos) {
                    std::string key = pair.substr(0, eq_pos);
                    std::string value = pair.substr(eq_pos + 1);
                    query_params[key] = value;
                }
            }
        }
        
        int room_id = std::stoi(query_params["room"]);
        std::string username = query_params["username"];

        // Handle WebSocket upgrade
        if (websocket::is_upgrade(req)) {
            std::cout << "LEt's upgrade connection" << std::endl;
            if (username.empty()) {
                http::response<http::string_body> bad_resp{http::status::bad_request, req.version()};
                bad_resp.set(http::field::content_type, "text/plain");
                bad_resp.body() = "Missing username";
                bad_resp.prepare_payload();
                http::write(socket, bad_resp);
                return;
            }

            std::cout << "Let's run websocket session" << std::endl;
            auto session = std::make_shared<WebSocketSession>(std::move(socket), room_id, username, ioc);
            session->run();
            return;
        }

        Response res;
        route_request(req, res, *db_conn);
        http::write(socket, res);
    } catch (const std::exception& e) {
        std::cerr << "Session error: " << e.what() << "\n";
    }
}
*/


// ------------------------------------------------------------
// Main entry point: sets up the server and listens for clients.
// ------------------------------------------------------------
int main(int argc, char* argv[]) {
    try {
        ConfigLoader config;
        config.load(".env");
    
        std::string db_host = config.get("DB_HOST");
        std::string port = config.get("PORT");

        std::cout << "Connecting to DB at: " << db_host << "\n";
        std::cout << "Server will start on port: " << port << "\n";

        // Shared so that all threads can access the db simultaneously
        // TODO: Needs testing for race conditions (need to ensure atomic db actions)
        auto db_conn = std::make_shared<pqxx::connection>("dbname=chatapp user=postgres password=secret host=localhost");

        auto doc_root = argv[3];

        // I/O execution context
        boost::asio::io_context ioc;

        // Open port 80
        tcp::endpoint endpoint{tcp::v4(), 8080};

        // Create and launch a listening port
        boost::make_shared<listener>(ioc, endpoint, boost::make_shared<shared_state>(doc_root))->run();

        std::cout << "Server is running at http://localhost:8080\n";
        std::cout << "HERE??";
        ioc.run();

        std::cout << "Now we crashing" << std::endl;        

    } catch (std::exception& e) {
        // Print errors to stderr if anything goes wrong
        std::cerr << "Error: " << e.what() << "\n";
    }
}