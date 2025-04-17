#pragma once

#include <unordered_map>
#include <set>
#include <mutex>
#include <memory>

// Forward declaration of WebSocketSession to avoid circular dependency.
// We only store shared_ptrs here, so we don't need the full definition.
class WebSocketSession;

/**
 * Singleton class that manages all chat rooms and their connected users (WebSocket sessions).
 * 
 * Each chat room is represented by an integer room ID and stores a set of sessions (users).
 * Thread-safe access is ensured via a mutex.
 */
class ChatRoomManager {
public:
    // Get the singleton instance of the chat room manager.
    static ChatRoomManager& get_instance();

    // Add a user session to a specific room.
    void join_room(int room_id, std::shared_ptr<WebSocketSession> session);

    // Remove a user session from a room. Deletes the room if it becomes empty.
    void leave_room(int room_id, std::shared_ptr<WebSocketSession> session);

    // Broadcast a message to all sessions in a room.
    void broadcast(int room_id, const std::string& message);

private:
    // Private constructor: ensures singleton pattern
    ChatRoomManager() = default;

    // Mutex to ensure thread-safe access to the rooms_ map.
    std::mutex mutex_;

    // Map from room ID to a set of connected sessions (users).
    std::unordered_map<int, std::set<std::shared_ptr<WebSocketSession>>> rooms_;
};