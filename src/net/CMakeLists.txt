if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(test)
endif ()

file(GLOB NET_HEADER_LIST "*.h")
file(GLOB NET_SRC_LIST "*.cc")


add_library(libnet_static STATIC
        ${NET_SRC_LIST}
        )

set_target_properties(libnet_static PROPERTIES OUTPUT_NAME "net")


install(FILES ${NET_HEADER_LIST}
        DESTINATION include/net
        )

install(TARGETS libnet_static
        ARCHIVE DESTINATION lib)