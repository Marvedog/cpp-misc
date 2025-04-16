#include "user.hpp"

#include <string>
#include <stdexcept>

namespace db {
    User::User(pqxx::connection& conn) : conn_(conn) {}

    std::optional<models::User> User::get_user_by_id(int id) {
        pqxx::work txn(conn_);
        auto result = txn.exec_params(
            "SELECT id, username FROM users WHERE id = $1", id
        );

        if (result.empty()) {
            return std::nullopt;
        }

        const auto& row = result[0];
        models::User user{
            row["id"].as<int>(),
            row["username"].as<std::string>()
        };
    
        return user;
    }

    std::vector<models::User> User::get_all_users() {
        pqxx::work txn(conn_);
        auto result = txn.exec("SELECT id, username FROM users");
    
        std::vector<models::User> users;
        for (const auto& row : result) {
            users.emplace_back(
                row["id"].as<int>(),
                row["username"].as<std::string>()
            );
        }
    
        return users;
    }

    void User::create_user(const models::User& user) {
        pqxx::work txn(conn_);
        txn.exec_params(
            "INSERT INTO users (username) VALUES ($1, $2)",
            user.username
        );
        txn.commit();
    }
}