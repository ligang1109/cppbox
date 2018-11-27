add_executable(misc_test
        misc_test.cc

        ${MISC_SRC_DIR}/misc.cc
        )

target_link_libraries(misc_test pthread)

add_executable(misc_simple_buffer_test
        simple_buffer_test.cc

        ${MISC_SRC_DIR}/simple_buffer.cc
        )

target_link_libraries(misc_simple_buffer_test pthread)