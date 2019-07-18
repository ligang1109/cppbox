//
// Created by ligang on 7/23/19.
//

#include "http_client.h"

namespace cppbox {

namespace net {


HttpClient::HttpClient(EventLoop *loop_ptr, TcpConnectionTimeWheel *time_wheel_ptr, size_t tcp_conn_pool_shard_size, size_t tcp_conn_pool_max_shard_cnt) :
        loop_ptr_(loop_ptr),
        time_wheel_ptr_(time_wheel_ptr) {}


}

}