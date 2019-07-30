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
        pool_index_(0) {}

void HttpClient::set_server_host(const std::string &host) {
  server_host_ = host;
}

void HttpClient::set_server_port(uint16_t port) {
  server_port_ = port;
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
  return std::make_shared<HttpConnection>(connfd, remote_addr, loop_ptr_);

}

}

}