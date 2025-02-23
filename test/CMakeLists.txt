cmake_minimum_required(VERSION 3.24)

project(test_chat_server CXX)

set(${PROJECT_NAME}_SRC
    test_chat_pcaket.cpp
    test_chat_room.cpp
    test_chat_server.cpp
    # test_chat_session.cpp
    test_user_service.cpp
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRC})
set_target_properties(${PROJECT_NAME} ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(${PROJECT_NAME} PRIVATE message_service gtest::gtest)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_test(NAME ${PROJECT_NAME}
    COMMAND ${PROJECT_NAME})
