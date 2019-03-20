set(MISC_HEADER_LIST
        ${MISC_SRC_DIR}/error.h
        ${MISC_SRC_DIR}/misc.h
        ${MISC_SRC_DIR}/non_copyable.h
        ${MISC_SRC_DIR}/simple_buffer.h
        ${MISC_SRC_DIR}/simple_time.h
        )

install(FILES ${MISC_HEADER_LIST}
        DESTINATION include/misc
        )


set(MISC_SRC_LIST
        ${MISC_SRC_DIR}/error.cc
        ${MISC_SRC_DIR}/misc.cc
        ${MISC_SRC_DIR}/simple_buffer.cc
        ${MISC_SRC_DIR}/simple_time.cc
        )

add_library(libmisc_static STATIC
        ${MISC_SRC_LIST}
        )

set_target_properties(libmisc_static PROPERTIES OUTPUT_NAME "misc")

install(TARGETS libmisc_static
        ARCHIVE DESTINATION lib)


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(test)
endif ()