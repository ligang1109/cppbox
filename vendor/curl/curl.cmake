include_directories(
        /usr/local/curl/include
)

link_libraries(
        /usr/local/curl/lib/libcurl.a
        /usr/local/openssl/lib/libssl.a
        /usr/local/openssl/lib/libcrypto.a
        /usr/local/zlib/lib/libz.a
        /usr/local/nghttp2/lib/libnghttp2.a
)
