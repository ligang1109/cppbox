set(NET_HEADER_LIST
        ${NET_SRC_DIR}/epoll.h
        ${NET_SRC_DIR}/event.h
        ${NET_SRC_DIR}/event_loop.h
        ${NET_SRC_DIR}/net.h
        ${NET_SRC_DIR}/trace_id_genter.h
        ${NET_SRC_DIR}/tcp_connection.h
        ${NET_SRC_DIR}/tcp_connection_time_wheel.h
        ${NET_SRC_DIR}/tcp_connection_pool.h
        ${NET_SRC_DIR}/tcp_server.h
        ${NET_SRC_DIR}/http_parse.h
        ${NET_SRC_DIR}/http_request.h
        ${NET_SRC_DIR}/http_response.h
        ${NET_SRC_DIR}/http_server.h
        )

install(FILES ${NET_HEADER_LIST}
        DESTINATION include/net
        )

set(NET_SRC_LIST
        ${NET_SRC_DIR}/epoll.cc
        ${NET_SRC_DIR}/event.cc
        ${NET_SRC_DIR}/event_loop.cc
        ${NET_SRC_DIR}/net.cc
        ${NET_SRC_DIR}/trace_id_genter.cc
        ${NET_SRC_DIR}/tcp_connection.cc
        ${NET_SRC_DIR}/tcp_connection_time_wheel.cc
        ${NET_SRC_DIR}/tcp_connection_pool.cc
        ${NET_SRC_DIR}/tcp_server.cc
        ${NET_SRC_DIR}/http_parse.cc
        ${NET_SRC_DIR}/http_request.cc
        ${NET_SRC_DIR}/http_response.cc
        ${NET_SRC_DIR}/http_server.cc

        ${VENDOR_DIR}/http-parser/http_parser.c
        )

add_library(libnet_static STATIC
        ${NET_SRC_LIST}
        )

set_target_properties(libnet_static PROPERTIES OUTPUT_NAME "net")

install(TARGETS libnet_static
        ARCHIVE DESTINATION lib)


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(test)
endif ()
