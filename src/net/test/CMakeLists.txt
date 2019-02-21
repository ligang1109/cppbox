link_libraries(
        ${BUILD_SRC_DIR}/net/libnet.a
        ${BUILD_SRC_DIR}/misc/libmisc.a
)

add_executable(net_test
        net_test.cc
        )

target_link_libraries(net_test pthread)

add_executable(net_tcp_connection_test
        tcp_connection_test.cc
        )

target_link_libraries(net_tcp_connection_test pthread)

add_executable(net_epoll_test
        epoll_test.cc
        )

target_link_libraries(net_epoll_test pthread)

add_executable(net_event_loop_test
        event_loop_test.cc
        )

target_link_libraries(net_event_loop_test pthread)

add_executable(net_tcp_server_test
        tcp_server_test.cc
        )

target_link_libraries(net_tcp_server_test pthread)

install(TARGETS
        net_test
        net_tcp_connection_test
        net_epoll_test
        net_event_loop_test
        net_tcp_server_test
        RUNTIME DESTINATION test/net)