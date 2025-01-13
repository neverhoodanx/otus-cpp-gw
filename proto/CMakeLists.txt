
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS chat_messages.proto )

add_library(chat_server_packets OBJECT ${LIBRARY_SOURCES} ${PROTO_SRCS} chat_messages.proto )
# set_target_properties(chat_server_packets PROPERTIES FOLDER ${NWSDK_PROJECTS_FOLDER})

target_include_directories(chat_server_packets
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(chat_server_packets PUBLIC protobuf::protobuf)

target_compile_definitions(chat_server_packets
        PRIVATE
        ${COMPILE_DEFINITIONS})

target_compile_options(chat_server_packets
        PRIVATE
        $<$<CONFIG:DEBUG>:${COMPILE_OPTIONS_DEBUG}>
        $<$<CONFIG:RELEASE>:${COMPILE_OPTIONS_RELEASE}>)

target_link_options(chat_server_packets
        PRIVATE
        $<$<CONFIG:DEBUG>:${LINK_OPTIONS_DEBUG}>
        $<$<CONFIG:RELEASE>:${LINK_OPTIONS_RELEASE}>)
