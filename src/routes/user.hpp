#pragma once

#include <string>

#include <pqxx/pqxx>

#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

using Request = http::request<http::string_body>;
using Response = http::response<http::string_body>;

namespace routes {
    void handle_user(Request& req, Response&res, pqxx::connection& conn);
}