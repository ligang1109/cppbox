//
// Created by ligang on 7/23/19.
//

#ifndef CPPBOX_NET_HTTP_CLIENT_H
#define CPPBOX_NET_HTTP_CLIENT_H


#include "cppbox/misc/non_copyable.h"

#include "event_loop.h"
#include "tcp_connection_time_wheel.h"
#include "tcp_connection_pool.h"

namespace cppbox {

namespace net {


class HttpClient : public misc::NonCopyable {
 public:
  explicit HttpClient(EventLoop *loop_ptr, TcpConnectionTimeWheel *time_wheel_ptr, size_t tcp_conn_pool_shard_size, size_t tcp_conn_pool_max_shard_cnt);

  void AddServerAddress();

 private:
  EventLoop *loop_ptr_;

  TcpConnectionTimeWheel *time_wheel_ptr_;

  std::map<std::string, TcpConnectionPoolUptr> pool_map_;
};


}

}


#endif //CPPBOX_NET_HTTP_CLIENT_H
