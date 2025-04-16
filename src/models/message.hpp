#pragma once

#include <nlohmann/json.hpp>

#include <string>

namespace models {

    struct Message {
        int id;
        int room_id;
        int user_id;
        std::string content;
        std::string timestamp;

        // Constructor to support instantiating via parameters
        Message(int id, int room_id, int user_id, std::string content, std::string timestamp)
        : id(id), room_id(room_id), user_id(user_id), content(std::move(content)), timestamp(timestamp) {}
    };

    inline void to_json(nlohmann::json& j, const Message& u) {
        j = nlohmann::json{
            {"id", u.id},
            {"room_id", u.room_id},
            {"user_id", u.user_id},
            {"content", u.content},
            {"timestamp", u.timestamp},
        };
    }
}