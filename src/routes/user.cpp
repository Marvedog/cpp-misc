#include "user.hpp"
#include "../db/user.hpp"

#include <boost/beast/http.hpp>

#include <regex>

namespace http = boost::beast::http;

using Request = http::request<http::string_body>;
using Response = http::response<http::string_body>;

namespace routes {
    void handle_user(Request& req, Response&res, pqxx::connection& conn) {
        db::User repo(conn);

        const std::string target = std::string(req.target());
        std::regex id_pattern(R"(^/v1/user/(\d+)$)");
        std::smatch match;

        if (std::regex_match(target, match, id_pattern) && req.method() == http::verb::get) {
            int user_id = std::stoi(match[1]);

            auto user = repo.get_user_by_id(user_id);

            res.set(http::field::content_type, "application/json");

            if (user) {
                res.result(http::status::ok);
                nlohmann::json j = *user;
                res.body() = j.dump();
            } else {
                res.result(http::status::not_found);
                res.body() = R"({"error": "User not found"})";
            }
            res.prepare_payload();
            return;
        } else if (target == "/v1/user" && req.method() == http::verb::get) {
            auto users = repo.get_all_users();

            nlohmann::json j = users;  // works with vector<User> because of to_json
            res.body() = j.dump();
    
            res.result(http::status::ok);
            res.set(http::field::content_type, "application/json");
            res.prepare_payload();
            return;
        } else if (req.method() == http::verb::post) {
            res.result(http::status::created);
            res.set(http::field::content_type, "application/json");
            res.body() = R"({"message": "CREATE user"})";
        } else if (req.method() == http::verb::put) {
            res.result(http::status::ok);
            res.set(http::field::content_type, "application/json");
            res.body() = R"({"message": "UPDATE user"})";
        } else if (req.method() == http::verb::delete_) {
            res.result(http::status::no_content);
            res.set(http::field::content_type, "application/json");
            res.body() = R"({"message": "DELETE user"})";
        } else {
            res.result(http::status::bad_request);
            res.body() = R"({"error": "Unsupported HTTP verb"})";
        }

        res.prepare_payload();
    }
}