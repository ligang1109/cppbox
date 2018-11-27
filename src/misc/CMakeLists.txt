if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(misc_test
            misc_test.cc
            misc.cc
            )

    target_link_libraries(misc_test pthread)
endif ()


add_library(libmisc_static STATIC
        misc.cc
        )

set_target_properties(libmisc_static PROPERTIES OUTPUT_NAME "misc")

install(TARGETS libmisc_static
        ARCHIVE DESTINATION lib)

install(FILES misc.h
        DESTINATION include/misc
        )