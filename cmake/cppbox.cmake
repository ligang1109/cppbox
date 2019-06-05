if (NOT CPPBOX_DIR)
    set(CPPBOX_DIR /usr/local/cppbox)
endif ()

include_directories(
        ${CPPBOX_DIR}/include
)

link_libraries(
        ${CPPBOX_DIR}/lib/libnet.a
        ${CPPBOX_DIR}/lib/liblog.a
        ${CPPBOX_DIR}/lib/libmisc.a
)
