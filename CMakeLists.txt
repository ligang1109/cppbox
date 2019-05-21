cmake_minimum_required(VERSION 3.10)
project(cppbox)

set(CMAKE_CXX_STANDARD 11)

set(CXX_FLAGS
        -Wall
        -Wextra
        -Werror
        -Wconversion
        -Wshadow
        )

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(BUILD_SRC_DIR ${PROJECT_BINARY_DIR}/src)
set(VENDOR_DIR ${PROJECT_SOURCE_DIR}/vendor)


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (NOT GTEST_DIR)
        set(GTEST_DIR /usr/local/googletest)
    endif ()

    include_directories(
            ${GTEST_DIR}/include
    )

    link_libraries(
            ${GTEST_DIR}/lib64/libgtest.a
    )
endif ()

include_directories(
        ${SRC_DIR}
        ${VENDOR_DIR}
)

add_subdirectory(${VENDOR_DIR})

set(MISC_SRC_DIR ${SRC_DIR}/misc)
add_subdirectory(${MISC_SRC_DIR})

set(LOG_SRC_DIR ${SRC_DIR}/log)
add_subdirectory(${LOG_SRC_DIR})

set(NET_SRC_DIR ${SRC_DIR}/net)
add_subdirectory(${NET_SRC_DIR})
