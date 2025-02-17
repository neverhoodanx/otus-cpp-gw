# otus-cpp-gw
Graduation qualifying work on the topic "Messenger - Instant Messaging Service"
---

## Requirements
 - conan 2.0.0
 - python 3.9
 - cmake 3.24 

## Server
A server for exchanging multi-link messages between users. It provides basic access to chat rooms for group conversations and also supports sending private messages.

### Usage
```sh 
$ message_service_srv <port>
```

### Build
```sh
$ make build # Build images
$ make run   # Start container
$ make logs  # View logs
$ make clean # Remove containers and images
```
### Options
 skip

---
## Test client
A simple console client that provides minimal working functionality for testing the chat server.

### Usage
```sh 
$ message_service_cli <nickname> <password> <host> <port>
```

### Commands
After connecting, you will receive a welcome message. All text that you send to the terminal will be sent to the server's chat room. By default, you will automatically join the "default room."
Available commands:
```sh 
/help
/room_list
/user_list
/join <room_name>
/leave
/whisper <nickname>
```