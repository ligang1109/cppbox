link_libraries(
        ${BUILD_SRC_DIR}/log/liblog.a
        ${BUILD_SRC_DIR}/misc/libmisc.a
)

add_executable(simple_logger_test
        simple_logger_test.cc
        )

target_link_libraries(simple_logger_test pthread)

add_executable(file_writer_test
        file_writer_test.cc
        )

target_link_libraries(file_writer_test pthread)

add_executable(console_writer_test
        console_writer_test.cc
        )

target_link_libraries(console_writer_test pthread)

add_executable(buffer_writer_test
        buffer_writer_test.cc
        )

target_link_libraries(buffer_writer_test pthread)

add_executable(simple_formater_test
        simple_formater_test.cc
        )

target_link_libraries(simple_formater_test pthread)

add_executable(async_writer_test
        async_writer_test.cc
        )

target_link_libraries(async_writer_test pthread)
