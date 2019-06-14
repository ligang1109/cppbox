if (NOT CURL_DIR)
    set(CURL_DIR /usr/local/curl)
endif ()

if (NOT OPENSSL_DIR)
    set(OPENSSL_DIR /usr/local/openssl)
endif ()

if (NOT ZLIB_DIR)
    set(ZLIB_DIR /usr/local/zlib)
endif ()

if (NOT NGHTTP2_DIR)
    set(NGHTTP2_DIR /usr/local/nghttp2)
endif ()


include_directories(
        ${CURL_DIR}/include
)

link_libraries(
        ${CURL_DIR}/lib/libcurl.a
        ${OPENSSL_DIR}/lib/libssl.a
        ${OPENSSL_DIR}/lib/libcrypto.a
        ${ZLIB_DIR}/lib/libz.a
        ${NGHTTP2_DIR}/lib/libnghttp2.a
)
