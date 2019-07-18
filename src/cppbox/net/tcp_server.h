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

#include "cppbox/log/base.h"

namespace cppbox {

namespace net {


int TcpConnectionThreadId();

class TcpServer : public misc::NonCopyable {
 public:
  using NewConnectionFunc = std::function<TcpConnectionSptr(int, const InetAddress &, EventLoop *)>;

  explicit TcpServer(uint16_t port, const std::string &ip = "127.0.0.1", uint16_t default_conn_idle_seconds = 300);

  std::string ip();

  uint16_t port();

  void set_logger_ptr(log::LoggerInterface *logger_ptr);

  void set_new_conn_func(const NewConnectionFunc &func);

  void set_connected_callback(const TcpConnectionCallback &cb);

  void set_disconnected_callback(const TcpConnectionCallback &cb);

  void set_read_callback(const TcpConnectionCallback &cb);

  void set_write_complete_callback(const TcpConnectionCallback &cb);

  void set_error_callback(const TcpConnectionCallback &cb);

  misc::ErrorUptr Init(int thread_cnt, int loop_timeout_ms, int init_evlist_size = 1024, size_t tcp_conn_pool_shard_size = 10000, size_t tcp_conn_pool_max_shard_cnt = 10);

  misc::ErrorUptr Start();

  size_t ConnectionThreadCount();

  size_t ConnectionCount();

  void RunFunctionInConnectionThread(int conn_thread_id, const EventLoop::Functor &func);

  EventLoop *ConnectionThreadLoop(int conn_thread_id);

  TcpConnectionTimeWheel *ConnectionThreadTimeWheel(int conn_thread_id);

 private:
  class ConnectionThread {
   public:
    explicit ConnectionThread(int id, TcpServer *server_ptr, size_t tcp_conn_pool_shard_size, size_t tcp_conn_pool_max_shard_cnt);

    misc::ErrorUptr Init(int loop_timeout_ms, int init_evlist_size = 1024);

    void Start();

    void AddConnection(int connfd, const InetAddress &remote_addr, const misc::SimpleTimeSptr &happen_st_sptr, const std::string &trace_id);

    size_t ConnectionCount();

    void RunFunction(const EventLoop::Functor &func);

    EventLoop *Loop();

    TcpConnectionTimeWheel *TimeWheel();

   private:
    void DisconnectedCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happen_st_sptr);

    static void TimeoutCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happen_st_sptr);

    void ConnectionReadCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happen_st_sptr);

    void UpdateActiveConnection(const TcpConnectionSptr &tcp_conn_sptr);

    void ThreadFunc();

    void InitConnectionCallback(const TcpConnectionSptr &tcp_conn_sptr);

    void AddConnectionInThread(int connfd, const InetAddress &remote_addr, const misc::SimpleTimeSptr &happen_st_sptr, const std::string &trace_id);

    int      id_;
    uint64_t conn_cnt_;

    TcpServer                    *server_ptr_;
    std::unique_ptr<std::thread> thread_uptr_;
    EventLoopUptr                loop_uptr_;

    TcpConnectionTimeWheelUptr time_wheel_uptr_;
    TcpConnectionPoolUptr      pool_uptr_;

    TcpConnectionCallback disconnected_callback_;
    TcpConnectionCallback connection_read_callback_;
    TcpConnectionCallback timeout_callback_;
  };

  misc::ErrorUptr ListenAndServe();

  void ListenCallback(const misc::SimpleTimeSptr &happen_st_sptr);

  static TcpConnectionSptr DefaultNewConnection(int connfd, const InetAddress &remote_addr, EventLoop *loop_ptr);

  std::string ip_;
  uint16_t    port_;

  log::LoggerInterface *logger_ptr_;
  TraceIdGenterUptr    trace_id_genter_uptr_;

  int           listenfd_;
  EventLoopUptr loop_uptr_;
  size_t        dispatch_index_;

  uint16_t                                       default_conn_idle_seconds_;
  std::vector<std::unique_ptr<ConnectionThread>> conn_thread_list_;

  NewConnectionFunc new_conn_func_;

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
