#include "health.hpp"
#include <boost/beast/http.hpp>
#include <string>

namespace http = boost::beast::http;

namespace routes {
    void handle_health(http::request<http::string_body>& req, http::response<http::string_body>& res) {
        if (req.method() == http::verb::get) {
            res.result(http::status::ok);
            res.set(http::field::content_type, "text/plain");
            res.body() = "OK";
        } else {
            res.result(http::status::method_not_allowed);
            res.body() = "Method not allowed";
        }  
    }
}