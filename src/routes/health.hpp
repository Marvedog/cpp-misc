#pragma once
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

namespace routes {
    void handle_health(http::request<http::string_body>& req, http::response<http::string_body>& res);
}