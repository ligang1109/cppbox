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
  using ResponseCallback = std::function<void(const HttpConnectionSptr &, bool is_timeout)>;

  explicit HttpClient(EventLoop *loop_ptr, TcpConnectionTimeWheel *time_wheel_ptr, size_t tcp_conn_pool_shard_size, size_t tcp_conn_pool_max_shard_cnt);

  void set_server_host(const std::string &host);

  void set_server_port(uint16_t port);

  void set_default_timeout_seconds(int timeout_seconds);

  void SetServerIpList(std::vector<std::string> &ip_list);

  void SetExpire(int expire_seconds, int expire_rate);

  void AddResponseCallback(const std::string &path, const ResponseCallback &cb);

  HttpConnectionSptr GetConnection();

  void PutConnection(HttpConnectionSptr &http_conn_sptr);

  void Do(HttpConnectionSptr &http_conn_sptr);

 private:
  void TimeoutCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happen_st_sptr);

  void ReadCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happen_st_sptr);

  void WriteCompleteCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happen_st_sptr);

  void RunResponseCallback(const TcpConnectionSptr &tcp_conn_sptr, bool is_timeout);

  EventLoop              *loop_ptr_;
  TcpConnectionTimeWheel *time_wheel_ptr_;

  size_t pool_shard_size_;
  size_t pool_max_shard_cnt_;
  size_t pool_index_;

  std::map<std::string, TcpConnectionPoolUptr> pool_map_;
  std::map<std::string, ResponseCallback>      response_callback_map_;

  std::string              server_host_;
  uint16_t                 server_port_;
  std::vector<std::string> server_ip_list_;

  int expire_seconds_;
  int expire_rate_;
  int expire_value_;

  int default_timeout_seconds_;
};


}

}


#endif //CPPBOX_NET_HTTP_CLIENT_H
