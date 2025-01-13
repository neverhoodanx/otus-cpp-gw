/**
 *  OTUS Graduate Work
 *  Main
 *  (c) 2025
 */

#include <iostream>
#include "chat_service.hpp"

int main(int argc, char* argv[]) {
  try {
    if (argc < 2) {
      std::cerr << "Usage: chat_server <port>\n";
      return 1;
    }
    
  uint16_t port = std::atoi(argv[1]);
  otus::chat_server::start(port);
  }
  catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}