cmake_minimum_required(VERSION 3.10)
project(cppbox)

set(CMAKE_CXX_STANDARD 11)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    include_directories(
            /usr/local/googletest-release-1.8.1/include
    )

    link_libraries(
            /usr/local/googletest-release-1.8.1/lib/libgtest.a
    )
endif ()

include_directories(
        ${SRC_DIR}
)

add_subdirectory(${SRC_DIR}/log)
add_subdirectory(${SRC_DIR}/misc)