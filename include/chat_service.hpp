/**
 *  OTUS Graduate Work
 *  Global chat server
 *  (c) 2025
 */
#pragma once

#include <cstdint>

namespace otus::chat_server {
/**
 * @brief Starts the chat server on the specified port
 *
 * This function initializes the server, creates the necessary rooms, and begins
 * listening for incoming client connections asynchronously
 *
 * @param port The port number on which the server should listen for connections
 */
void start(uint16_t port);
/**
 * @brief Stops the chat server
 *
 * This function performs a graceful shutdown, notifying all connected clients
 * and stopping the server instance
 */
void stop();

} // namespace otus::chat_server