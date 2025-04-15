#pragma once

#include <nlohmann/json.hpp>

#include <string>

namespace models {

    struct User {
        int id;
        std::string username;
        std::string email;

        // Constructor to support instantiating via parameters
        User(int id, std::string username, std::string email)
        : id(id), username(std::move(username)), email(std::move(email)) {}
    };

    inline void to_json(nlohmann::json& j, const User& u) {
        j = nlohmann::json{
            {"id", u.id},
            {"username", u.username},
            {"email", u.email}
        };
    }
}