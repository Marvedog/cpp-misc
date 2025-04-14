#include "user.hpp"
#include <boost/beast/http.hpp>
#include <regex>

namespace http = boost::beast::http;

namespace routes {
    void handle_user_request(http::request<http::string_body>& req, http::response<http::string_body>&res) {
        if (req.method() == http::verb::get) {
            res.result(http::status::ok);
            res.set(http::field::content_type, "application/json");
            res.body() = R"({"message": "GET user"})";
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