#pragma once

#include <nlohmann/json.hpp>

#include <string>

namespace models {

    struct User {
        int id;
        std::string username;

        // Constructor to support instantiating via parameters
        User(int id, std::string username)
        : id(id), username(std::move(username)) {}
    };

    inline void to_json(nlohmann::json& j, const User& u) {
        j = nlohmann::json{
            {"id", u.id},
            {"username", u.username},
        };
    }
}