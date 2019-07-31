//
// Created by ligang on 7/23/19.
//

#include "http_client.h"

#include <algorithm>

namespace cppbox {

namespace net {


HttpClient::HttpClient(EventLoop *loop_ptr, TcpConnectionTimeWheel *time_wheel_ptr, size_t tcp_conn_pool_shard_size, size_t tcp_conn_pool_max_shard_cnt) :
        loop_ptr_(loop_ptr),
        time_wheel_ptr_(time_wheel_ptr),
        pool_shard_size_(tcp_conn_pool_shard_size),
        pool_max_shard_cnt_(tcp_conn_pool_max_shard_cnt),
        pool_index_(0),
        expire_seconds_(300),
        expire_rate_(8),
        expire_value_(-1) {}

void HttpClient::set_server_host(const std::string &host) {
  server_host_ = host;
}

void HttpClient::set_server_port(uint16_t port) {
  server_port_ = port;
}

void HttpClient::set_default_timeout_seconds(int timeout_seconds) {
  default_timeout_seconds_ = timeout_seconds;
}

void HttpClient::SetServerIpList(std::vector<std::string> &ip_list) {
  server_ip_list_ = ip_list;
  for (auto it = pool_map_.begin(); it != pool_map_.end();) {
    auto find = std::find(server_ip_list_.begin(), server_ip_list_.end(), it->first);
    if (find == server_ip_list_.end()) {
      pool_map_.erase(it++);
    } else {
      ++it;
    }
  }

  for (auto &ip : server_ip_list_) {
    auto it = pool_map_.find(ip);
    if (it == pool_map_.end()) {
      pool_map_.emplace(ip, std::unique_ptr<TcpConnectionPool>(new TcpConnectionPool(pool_shard_size_, pool_max_shard_cnt_)));
    }
  }

  if (pool_index_ >= server_ip_list_.size()) {
    pool_index_ %= server_ip_list_.size();
  }
}

void HttpClient::SetExpire(int expire_seconds, int expire_rate) {
  expire_seconds_ = expire_seconds;
  expire_rate_    = expire_rate;
}

void HttpClient::AddResponseCallback(const std::string &path, const ResponseCallback &cb) {
  auto it = response_callback_map_.find(path);
  if (it == response_callback_map_.end()) {
    response_callback_map_.emplace(path, cb);
  } else {
    it->second = cb;
  }
}

HttpConnectionSptr HttpClient::GetConnection() {
  auto it            = pool_map_.find(server_ip_list_[pool_index_]);
  auto tcp_conn_sptr = it->second->Get();
  if (tcp_conn_sptr != nullptr) {
    return std::static_pointer_cast<HttpConnection>(tcp_conn_sptr);
  }

  auto connfd = NewTcpIpV4NonBlockSocket();
  if (connfd == -1) {
    return nullptr;
  }

  InetAddress remote_addr{server_ip_list_[pool_index_], server_port_};
  pool_index_ = (pool_index_ + 1) % server_ip_list_.size();

  return std::make_shared<HttpConnection>(connfd, remote_addr, loop_ptr_);
}

void HttpClient::PutConnection(HttpConnectionSptr &http_conn_sptr) {
  if (http_conn_sptr->last_receive_time_sptr()->Sec() - http_conn_sptr->connected_time_sptr()->Sec() > expire_seconds_) {
    expire_value_ = (expire_value_ + 1) % 10;
    if (expire_value_ < expire_rate_) {
      http_conn_sptr->ForceClose();
      return;
    }
  }

  auto it = pool_map_.find(http_conn_sptr->remote_ip());
  if (it == pool_map_.end()) {
    http_conn_sptr->ForceClose();
    return;
  }

  http_conn_sptr->ResetMore();
  if (!it->second->Put(http_conn_sptr)) {
    http_conn_sptr->ForceClose();
  }
}

void HttpClient::Do(HttpConnectionSptr &http_conn_sptr) {

}


void HttpClient::TimeoutCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happen_st_sptr) {
  RunResponseCallback(tcp_conn_sptr, true);
}

void HttpClient::ReadCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happen_st_sptr) {

}

void HttpClient::RunResponseCallback(const TcpConnectionSptr &tcp_conn_sptr, bool is_timeout) {
  auto http_conn_sptr = std::static_pointer_cast<HttpConnection>(tcp_conn_sptr);
  auto path           = http_conn_sptr->Request()->raw_path();

  auto it = response_callback_map_.find(path);
  if (it != response_callback_map_.end()) {
    it->second(http_conn_sptr, is_timeout);
  }
}


}

}