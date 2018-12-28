link_libraries(
        ${BUILD_SRC_DIR}/misc/libmisc.a
)

add_executable(misc_test
        misc_test.cc
        )

target_link_libraries(misc_test pthread)

add_executable(misc_simple_buffer_test
        simple_buffer_test.cc
        )

target_link_libraries(misc_simple_buffer_test pthread)

add_executable(misc_error_test
        error_test.cc
        )

target_link_libraries(misc_error_test pthread)

add_executable(misc_simple_time_test
        simple_time_test.cc
        )

target_link_libraries(misc_simple_time_test pthread)