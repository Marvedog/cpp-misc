#pragma once
#include <boost/beast/http.hpp>
#include <string>

namespace http = boost::beast::http;

namespace routes {
    void handle_user_request(http::request<http::string_body>& req, http::response<http::string_body>& res);
}