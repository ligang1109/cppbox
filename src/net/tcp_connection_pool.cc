//
// Created by ligang on 5/24/19.
//

#include "tcp_connection_pool.h"

namespace cppbox {

namespace net {


TcpConnectionPool::TcpConnectionPool(size_t shard_size, size_t max_shard_cnt) :
        pool_(1),
        shard_size_(shard_size),
        max_shard_cnt_(max_shard_cnt),
        index_(0) {
  pool_[0].reserve(shard_size_);
}

size_t TcpConnectionPool::shard_size() {
  return shard_size_;
}

size_t TcpConnectionPool::max_shard_cnt() {
  return max_shard_cnt_;
}

bool TcpConnectionPool::Full() {
  return index_ == max_shard_cnt_;
}

bool TcpConnectionPool::Empty() {
  return index_ == 0 && pool_[0].empty();
}

bool TcpConnectionPool::Put(const TcpConnectionSptr &tcp_conn_sptr) {
  if (Full()) {
    return false;
  }

  pool_[index_].push_back(tcp_conn_sptr);

  if (pool_[index_].size() == shard_size_) {
    ++index_;
    if (index_ < max_shard_cnt_ && index_ == pool_.size()) {
      pool_.emplace_back(std::vector<TcpConnectionSptr>());
      pool_[index_].reserve(shard_size_);
    }
  }

  return true;
}

TcpConnectionSptr TcpConnectionPool::Get() {
  if (Empty()) {
    return nullptr;
  }

  if (Full()) {
    --index_;
  }

  auto tcp_conn_sptr = pool_[index_].back();
  pool_[index_].pop_back();

  if (pool_[index_].empty()) {
    if (index_ > 0) {
      --index_;
    }
  }

  return tcp_conn_sptr;
}


}

}