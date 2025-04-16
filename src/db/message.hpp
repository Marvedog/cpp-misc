#pragma once

#include "../models/message.hpp"

#include <pqxx/pqxx>

#include <vector>

namespace db {

class Message {
    public:
        explicit Message(pqxx::connection& conn);

        std::vector<models::Message> get_messages_for_room(int room_id);
        void save(const models::Message& message);

    private:
        pqxx::connection& conn_;
};

}