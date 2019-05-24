//
// Created by ligang on 19-2-3.
//

#ifndef CPPBOX_NET_TCP_SERVER_H
#define CPPBOX_NET_TCP_SERVER_H

#include <vector>
#include <thread>

#include "tcp_connection.h"
#include "tcp_connection_time_wheel.h"
#include "tcp_connection_pool.h"
#include "trace_id_genter.h"

#include "log/base.h"

namespace cppbox {

namespace net {


int TcpConnectionThreadId();

class TcpServer : public misc::NonCopyable {
 public:
  explicit TcpServer(uint16_t port, const std::string &ip = "127.0.0.1", uint16_t default_conn_idle_seconds = 300);

  std::string ip();

  uint16_t port();

  void set_logger_ptr(log::LoggerInterface *logger_ptr);

  void set_connected_callback(const TcpConnectionCallback &cb);

  void set_disconnected_callback(const TcpConnectionCallback &cb);

  void set_read_callback(const TcpConnectionCallback &cb);

  void set_write_complete_callback(const TcpConnectionCallback &cb);

  void set_error_callback(const TcpConnectionCallback &cb);

  misc::ErrorUptr Init(int thread_cnt, int loop_timeout_ms, int init_evlist_size = 1024, size_t tcp_conn_pool_shard_size = 10000, size_t tcp_conn_pool_max_shard_cnt = 10);

  misc::ErrorUptr Start();

  size_t ConnectionCount();

  void RunFunctionInConnectionThread(int conn_thread_id, const EventLoop::Functor &func);

 private:
  class ConnectionThread {
   public:
    explicit ConnectionThread(int id, TcpServer *server_ptr, size_t tcp_conn_pool_shard_size, size_t tcp_conn_pool_max_shard_cnt);

    misc::ErrorUptr Init(int loop_timeout_ms, int init_evlist_size = 1024);

    void Start();

    void AddConnection(int connfd, const InetAddress &remote_addr, const misc::SimpleTimeSptr &happened_st_sptr, const std::string &trace_id);

    size_t ConnectionCount();

    void RunFunction(const EventLoop::Functor &func);

   private:
    void DisconnectedCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happened_st_sptr);

    void ConnectionReadCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happened_st_sptr);

    void UpdateActiveConnection(const TcpConnectionSptr &tcp_conn_sptr);

    void ThreadFunc();

    void AddConnectionInThread(int connfd, const InetAddress &remote_addr, const misc::SimpleTimeSptr &happened_st_sptr, const std::string &trace_id);

    int id_;

    TcpServer                    *server_ptr_;
    std::unique_ptr<std::thread> thread_uptr_;
    EventLoopUptr                loop_uptr_;

    TcpConnectionTimeWheelUptr time_wheel_uptr_;
    std::map<int, uint16_t>    conn_time_hand_map_;

    TcpConnectionPoolUptr pool_uptr_;
  };

  misc::ErrorUptr ListenAndServe();

  void ListenCallback(const misc::SimpleTimeSptr &happened_st_sptr);

  std::string ip_;
  uint16_t    port_;

  log::LoggerInterface *logger_ptr_;
  TraceIdGenterUptr    trace_id_genter_uptr_;

  int           listenfd_;
  EventLoopUptr loop_uptr_;
  size_t        dispatch_index_;

  uint16_t                                       default_conn_idle_seconds_;
  std::vector<std::unique_ptr<ConnectionThread>> conn_thread_list_;

  TcpConnectionCallback connected_callback_;
  TcpConnectionCallback disconnected_callback_;
  TcpConnectionCallback read_callback_;
  TcpConnectionCallback write_complete_callback_;
  TcpConnectionCallback error_callback_;
};

using TcpServerUptr = std::unique_ptr<TcpServer>;


}

}


#endif //CPPBOX_NET_TCP_SERVER_H
