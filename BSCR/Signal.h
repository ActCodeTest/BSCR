#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <vector>
#include <algorithm>

namespace HKUltra {

    /*
    * Signal class represents a mechanism for notifying a list of connected functions (slots).
    * It is used to connect, disconnect, and emit signals to those functions.
    * The class does not manage the lifetime of the connected functions (slots), and it's up to the user
    * to ensure that they are valid when emitting the signal.
    */
    template<typename... _args>
    class Signal {
    public:
        // SlotType is a function signature that represents the connected function (slot)
        // It accepts a variable number of arguments (_args...) and returns void.
        typedef std::function<void(_args...)> SlotType;

        // ConnectionType is a shared pointer to a SlotType, representing a connection
        // to a particular function (slot).
        typedef std::shared_ptr<SlotType> ConnectionType;

        /*
        * Connects a slot (function) to the signal. The function is wrapped in a shared_ptr
        * so that the signal can maintain a reference to it. Returns a ConnectionType to manage
        * the connection, which can be used later to disconnect the slot.
        */
        ConnectionType connect(SlotType slot) {
            // Create a shared pointer to the slot
            ConnectionType connection = std::make_shared<SlotType>(std::move(slot));

            {   // Lock for thread safety: ensures that no other thread modifies the connection list while we're adding to it
                std::lock_guard<std::mutex> lock(mtx_);
                connections_.push_back(connection);  // Add the connection to the list
            }

            return connection; // Return the connection to the caller for potential future disconnection
        }

        /*
        * Disconnects a slot (function) from the signal. The connection is identified by the
        * ConnectionType returned when the slot was originally connected.
        */
        void disconnect(ConnectionType connection) {
            {   // Lock for thread safety: prevents concurrent access/modification of the connection list
                std::lock_guard<std::mutex> lock(mtx_);

                // Condition to check if a weak pointer's locked reference matches the connection
                auto condition = [&connection](const std::weak_ptr<SlotType>& weak_connection) {
                    return weak_connection.lock() == connection;  // Check if the weak pointer matches the shared pointer
                    };

                // Erase the connection from the list if the condition is true
                connections_.erase(std::remove_if(connections_.begin(), connections_.end(), condition), connections_.end());
            }
        }

        /*
        * Emits a signal to all connected slots, passing the provided arguments (_args...).
        * Each connected slot will be called with the arguments provided.
        * If a slot has been disconnected or its shared pointer expired, it will be removed from the list.
        */
        void emit(_args... args) {
            {   // Lock for thread safety: prevents other threads from modifying the connection list while emitting the signal
                std::lock_guard<std::mutex> lock(mtx_);

                // Condition to check if a weak pointer is still valid (i.e., the slot has not been destroyed)
                auto condition = [&](const std::weak_ptr<SlotType>& weak_connection) {
                    if (auto connection = weak_connection.lock()) {  // If the slot is still valid (i.e., shared_ptr is still alive)
                        (*connection)(args...);  // Call the slot (function) with the arguments
                        return false;  // Keep the connection in the list (we do not remove it just yet)
                    }
                    return true;  // If the slot is no longer valid, mark it for removal
                    };

                // Remove invalid connections (expired or disconnected slots)
                connections_.erase(std::remove_if(connections_.begin(), connections_.end(), condition), connections_.end());
            }
        }

    private:
        mutable std::mutex mtx_;  // Mutex to ensure thread safety when modifying the connections list
        std::vector<std::weak_ptr<SlotType>> connections_;  // List of weak pointers to the connected slots (functions)
    };

}  // namespace HKUltra