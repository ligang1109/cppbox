set(LOG_HEADER_LIST
        ${LOG_SRC_DIR}/async_writer.h
        ${LOG_SRC_DIR}/base.h
        ${LOG_SRC_DIR}/console_writer.h
        ${LOG_SRC_DIR}/file_writer.h
        ${LOG_SRC_DIR}/simple_formater.h
        ${LOG_SRC_DIR}/simple_logger.h
        )

install(FILES ${LOG_HEADER_LIST}
        DESTINATION ${HEADER_INSTALL_PREFIX}/log
        )


set(LOG_SRC_LIST
        ${LOG_SRC_DIR}/async_writer.cc
        ${LOG_SRC_DIR}/console_writer.cc
        ${LOG_SRC_DIR}/file_writer.cc
        ${LOG_SRC_DIR}/simple_formater.cc
        ${LOG_SRC_DIR}/simple_logger.cc
        )

add_library(liblog_static STATIC
        ${LOG_SRC_LIST}
        )

set_target_properties(liblog_static PROPERTIES OUTPUT_NAME "log")

install(TARGETS liblog_static
        ARCHIVE DESTINATION lib)


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(test)
endif ()