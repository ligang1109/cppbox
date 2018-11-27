if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(test)
endif ()

file(GLOB LOG_HEADER_LIST "*.h")
file(GLOB LOG_SRC_LIST "*.cc")


add_library(liblog_static STATIC
        ${LOG_SRC_LIST}
        )

set_target_properties(liblog_static PROPERTIES OUTPUT_NAME "log")


install(FILES ${LOG_HEADER_LIST}
        DESTINATION include/log
        )

install(TARGETS liblog_static
        ARCHIVE DESTINATION lib)