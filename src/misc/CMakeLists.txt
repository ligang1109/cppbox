if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(test)
endif ()

file(GLOB MISC_HEADER_LIST "*.h")
file(GLOB MISC_SRC_LIST "*.cc")


add_library(libmisc_static STATIC
        ${MISC_SRC_LIST}
        )

set_target_properties(libmisc_static PROPERTIES OUTPUT_NAME "misc")


install(FILES ${MISC_HEADER_LIST}
        DESTINATION include/misc
        )

install(TARGETS libmisc_static
        ARCHIVE DESTINATION lib)