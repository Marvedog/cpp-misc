#pragma once

#include <nlohmann/json.hpp>

#include <string>

namespace models {

    struct Chat {
        int id;
        std::string name;

        // Constructor to support instantiating via parameters
        Chat(int id, std::string name)
        : id(id), name(std::move(name)) {}
    };

    inline void to_json(nlohmann::json& j, const Chat& u) {
        j = nlohmann::json{
            {"id", u.id},
            {"name", u.name},
        };
    }
}