link_libraries(
        ${BUILD_SRC_DIR}/log/liblog.a
        ${BUILD_SRC_DIR}/misc/libmisc.a
)

add_executable(log_simple_logger_test
        simple_logger_test.cc
        )

target_link_libraries(log_simple_logger_test pthread)

add_executable(log_file_writer_test
        file_writer_test.cc
        )

target_link_libraries(log_file_writer_test pthread)

add_executable(log_console_writer_test
        console_writer_test.cc
        )

target_link_libraries(log_console_writer_test pthread)

add_executable(log_buffer_writer_test
        buffer_writer_test.cc
        )

target_link_libraries(log_buffer_writer_test pthread)

add_executable(log_simple_formater_test
        simple_formater_test.cc
        )

target_link_libraries(log_simple_formater_test pthread)

add_executable(log_async_writer_test
        async_writer_test.cc
        )

target_link_libraries(log_async_writer_test pthread)
