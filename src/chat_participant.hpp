/**
 *  OTUS Graduate Work
 *  Participant in a chat
 *  (c) 2025
 */
#pragma once

#include <string>

namespace otus::chat_server {
/**
 * @brief Abstract base class representing a participant in a chat.
 * 
 * The chat_participant class serves as an interface for participants in a chat room.
 * It requires derived classes to implement the deliver method to handle incoming messages.
 */
class chat_participant {
public:
  /**
   * @brief Dtor
   */
  virtual ~chat_participant() = default;

  /**
   * @brief Deliver a message to the chat participant.
   * @param msg Message to deliver.
   */
  virtual void deliver(const std::string& msg) = 0;
};
} // namespace otus::chat_server