if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(simple_logger_test
            simple_logger_test.cc
            simple_logger.cc
            )

    target_link_libraries(simple_logger_test pthread)

    add_executable(file_writer_test
            file_writer_test.cc
            file_writer.cc

            ${SRC_DIR}/misc/misc.cc
            )

    target_link_libraries(file_writer_test pthread)

    add_executable(console_writer_test
            console_writer_test.cc
            console_writer.cc
            )

    target_link_libraries(console_writer_test pthread)
endif ()