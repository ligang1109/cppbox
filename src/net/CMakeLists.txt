set(NET_HEADER_LIST
        ${NET_SRC_DIR}/epoll.h
        ${NET_SRC_DIR}/event.h
        ${NET_SRC_DIR}/event_loop.h
        ${NET_SRC_DIR}/net.h
        ${NET_SRC_DIR}/tcp_connection.h
        ${NET_SRC_DIR}/tcp_server.h
        )

set(NET_SRC_LIST
        ${NET_SRC_DIR}/epoll.cc
        ${NET_SRC_DIR}/event.cc
        ${NET_SRC_DIR}/event_loop.cc
        ${NET_SRC_DIR}/net.cc
        ${NET_SRC_DIR}/tcp_connection.cc
        ${NET_SRC_DIR}/tcp_server.cc
        )

add_library(libnet_static STATIC
        ${NET_SRC_LIST}
        )

set_target_properties(libnet_static PROPERTIES OUTPUT_NAME "net")


install(FILES ${NET_HEADER_LIST}
        DESTINATION include/net
        )

install(TARGETS libnet_static
        ARCHIVE DESTINATION lib)


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(test)
endif ()