#include "chat_room_manager.hpp"
#include "ws/session.hpp" // Needed here for full definition of WebSocketSession

// Static method that returns a reference to the singleton instance.
// Initialized the first time it's called — thread-safe in modern C++.
ChatRoomManager& ChatRoomManager::get_instance() {
    static ChatRoomManager instance;
    return instance;
}

void ChatRoomManager::join_room(int room_id, std::shared_ptr<WebSocketSession> session) {
    std::lock_guard<std::mutex> lock(mutex_);
    rooms_[room_id].insert(session); // Insert the session into the room's set
}

void ChatRoomManager::leave_room(int room_id, std::shared_ptr<WebSocketSession> session) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto& room = rooms_[room_id];
    room.erase(session); // Remove session
    if (room.empty()) {
        rooms_.erase(room_id); // Clean up if the room is now empty
    }
}

void ChatRoomManager::broadcast(int room_id, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = rooms_.find(room_id);
    if (it != rooms_.end()) {
        // Send the message to every connected session in the room
        for (auto& session : it->second) {
            //session->send(message);
        }
    }
}