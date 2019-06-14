//
// Created by ligang on 5/24/19.
//

#ifndef CPPBOX_NET_TCP_CONNECTION_POOL_H
#define CPPBOX_NET_TCP_CONNECTION_POOL_H


#include <vector>

#include "tcp_connection.h"

namespace cppbox {

namespace net {


class TcpConnectionPool {
 public:
  explicit TcpConnectionPool(size_t shard_size = 10000, size_t max_shard_cnt = 10);

  size_t shard_size();

  size_t max_shard_cnt();

  bool Full();

  bool Empty();

  bool Put(const TcpConnectionSptr &tcp_conn_sptr);

  TcpConnectionSptr Get();

 private:
  std::vector<std::vector<TcpConnectionSptr>> pool_;

  size_t shard_size_;
  size_t max_shard_cnt_;
  size_t index_;
};

using TcpConnectionPoolUptr = std::unique_ptr<TcpConnectionPool>;


}

}


#endif //CPPBOX_NET_TCP_CONNECTION_POOL_H
