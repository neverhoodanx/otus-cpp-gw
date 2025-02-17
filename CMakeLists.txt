cmake_minimum_required(VERSION 3.24)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 1.1.${PATCH_VERSION})

project(message_service VERSION ${PROJECT_VESRION})

option(WITH_CLIENT_TEST "Build test client" ON)

configure_file(version.h.in version.h)

set(${PROJECT_NAME}_SRC
    src/chat_service.cpp include/chat_service.hpp
    src/chat_network.cpp src/chat_network.hpp
    src/chat_session.cpp src/chat_session.hpp
    src/chat_server.cpp src/chat_server.hpp
    src/chat_room.cpp src/chat_room.hpp
    src/chat_packet.cpp src/chat_packet.hpp
    src/chat_participant.hpp
    src/user_service.cpp src/user_service.hpp
    src/user_info.hpp
)

add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SRC})
set_target_properties(${PROJECT_NAME} ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PRIVATE src)


add_executable(${PROJECT_NAME}_srv main.cpp)
set_target_properties(${PROJECT_NAME}_srv ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
target_link_libraries(${PROJECT_NAME}_srv PRIVATE ${PROJECT_NAME})

add_executable(${PROJECT_NAME}_cli client.cpp src/chat_packet.cpp)
set_target_properties(${PROJECT_NAME}_cli ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

find_package(asio REQUIRED)
find_package(GTest REQUIRED)
find_package(protobuf 5.27.0 REQUIRED)

set(Protobuf_PROTOC_EXECUTABLE "/Users/neverhooda/.conan2/p/b/proto6ac2ef8bd64dd/p/bin/protoc")

target_link_libraries(${PROJECT_NAME} PUBLIC asio::asio)
target_link_libraries(${PROJECT_NAME}_cli PUBLIC asio::asio)

add_subdirectory(proto)

target_link_libraries(${PROJECT_NAME} PUBLIC chat_server_packets)
target_link_libraries(${PROJECT_NAME}_cli PUBLIC chat_server_packets)


if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4
    )
    target_compile_options(${PROJECT_NAME}_srv PRIVATE
        /W4
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(${PROJECT_NAME}_srv PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()

enable_testing()
add_subdirectory(test)
