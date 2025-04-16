#include "message.hpp"

#include <string>
#include <stdexcept>

namespace db {
    Message::Message(pqxx::connection& conn) : conn_(conn) {}

    std::vector<models::Message> Message::get_messages_for_room(int room_id) {
        pqxx::work txn(conn_);
        auto result = txn.exec_params(
            "SELECT id, room_id, user_id, content, timestamp FROM message WHERE room_id = $1 ORDER BY timestamp ASC",
            room_id
        );
    
        std::vector<models::Message> messages;
        for (const auto& row : result) {
            messages.push_back({
                row["id"].as<int>(),
                row["room_id"].as<int>(),
                row["user_id"].as<int>(),
                row["content"].as<std::string>(),
                row["timestamp"].as<std::string>()
            });
        }
    
        return messages;
    }

    void Message::save(const models::Message& msg) {
        pqxx::work txn(conn_);
        txn.exec_params(
            "INSERT INTO message (room_id, user_id, content) VALUES ($1, $2, $3)",
            msg.room_id, msg.user_id, msg.content
        );
        txn.commit();
    }
}