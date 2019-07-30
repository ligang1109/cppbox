//
// Created by ligang on 7/23/19.
//

#ifndef CPPBOX_NET_HTTP_CLIENT_H
#define CPPBOX_NET_HTTP_CLIENT_H


#include "cppbox/misc/non_copyable.h"

#include "event_loop.h"
#include "tcp_connection_time_wheel.h"
#include "tcp_connection_pool.h"
#include "http_connection.h"

namespace cppbox {

namespace net {


class HttpClient : public misc::NonCopyable {
 public:
  explicit HttpClient(EventLoop *loop_ptr, TcpConnectionTimeWheel *time_wheel_ptr, size_t tcp_conn_pool_shard_size, size_t tcp_conn_pool_max_shard_cnt);

  void set_server_host(const std::string &host);

  void set_server_port(uint16_t port);

  void SetServerIpList(std::vector<std::string> &ip_list);

  HttpConnectionSptr GetConnection();

 private:
  EventLoop              *loop_ptr_;
  TcpConnectionTimeWheel *time_wheel_ptr_;

  size_t pool_shard_size_;
  size_t pool_max_shard_cnt_;
  size_t pool_index_;

  std::map<std::string, TcpConnectionPoolUptr> pool_map_;

  std::string              server_host_;
  uint16_t                 server_port_;
  std::vector<std::string> server_ip_list_;
};


}

}


#endif //CPPBOX_NET_HTTP_CLIENT_H
