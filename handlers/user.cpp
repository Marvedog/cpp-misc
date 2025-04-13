#include "user.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <fmt/core.h>
#include <pqxx/pqxx>
#include <regex>
#include <iostream>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

namespace handlers {

void handle_session(tcp::socket socket) {
    try {
        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(socket, buffer, req);

        http::response<http::string_body> res;
        res.version(req.version());
        res.set(http::field::server, "Beast/Boost C++ REST");
        res.keep_alive(req.keep_alive());

        std::string target_str{req.target()};
        std::smatch match;
        std::regex user_regex(R"(^/v1/user/(\d+)$)");

        if (req.method() == http::verb::get && std::regex_match(target_str, match, user_regex)) {
            int user_id = std::stoi(match[1].str());

            try {
                pqxx::connection db_conn{"postgresql://postgres:password@localhost:5432/myappdb"};
                pqxx::work txn{db_conn};

                pqxx::result r = txn.exec_params(
                    "SELECT id, username, email, created_at FROM users WHERE id = $1", user_id
                );

                if (!r.empty()) {
                    auto row = r[0];
                    std::string body = fmt::format(
                        R"({{"id":{},"username":"{}","email":"{}","created_at":"{}"}})",
                        row["id"].as<int>(),
                        row["username"].c_str(),
                        row["email"].c_str(),
                        row["created_at"].c_str()
                    );

                    res.result(http::status::ok);
                    res.set(http::field::content_type, "application/json");
                    res.body() = body;
                } else {
                    res.result(http::status::not_found);
                    res.body() = R"({"error":"User not found"})";
                    res.set(http::field::content_type, "application/json");
                }

                res.prepare_payload();
            } catch (const std::exception& e) {
                res.result(http::status::internal_server_error);
                res.body() = fmt::format(R"({{"error":"Database error: {}"}})", e.what());
                res.set(http::field::content_type, "application/json");
                res.prepare_payload();
            }
        } else {
            res.result(http::status::not_found);
            res.body() = "Route not found";
            res.set(http::field::content_type, "text/plain");
            res.prepare_payload();
        }

        http::write(socket, res);
    } catch (const std::exception& e) {
        std::cerr << "Session error: " << e.what() << "\n";
    }
}

} // namespace handlers