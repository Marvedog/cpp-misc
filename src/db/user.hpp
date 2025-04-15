#pragma once

#include "../models/user.hpp"

#include <pqxx/pqxx>

#include <optional>
#include <vector>

namespace db {

class User {
    public:
        explicit User(pqxx::connection& conn);

        std::optional<models::User> get_user_by_id(int id);
        std::vector<models::User> get_all_users();
        void create_user(const models::User& user);

    private:
        pqxx::connection& conn_;
};

}