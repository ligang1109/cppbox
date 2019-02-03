//
// Created by ligang on 19-2-3.
//

#ifndef CPPBOX_NET_TCP_SERVER_H
#define CPPBOX_NET_TCP_SERVER_H

#include <vector>
#include <thread>

#include "tcp_connection.h"

namespace cppbox {

namespace net {

__thread int tcp_conn_thread_id;

int TcpConnectionThreadId();

class TcpServer : public misc::NonCopyable {
 public:
  explicit TcpServer(uint16_t port, const log::LoggerSptr &logger_sptr = nullptr, const char *ip = "127.0.0.1");

  void set_connected_callback(const TcpConnCallback &cb);

  void set_disconnected_callback(const TcpConnCallback &cb);

  void set_read_callback(const TcpConnCallback &cb);

  void set_write_complete_callback(const TcpConnCallback &cb);

  void set_error_callback(const TcpConnCallback &cb);

  misc::ErrorUptr Init(int conn_thread_cnt, int conn_thread_loop_timeout_ms);

  misc::ErrorUptr Start();

  size_t ConnectionCount();

  void RunFunctionInConnectionThread(int conn_thread_id, const EventLoop::Functor &func);

 private:
  class ConnectionThread {
   public:
    explicit ConnectionThread(int id, TcpServer *server_ptr);

    misc::ErrorUptr Init(int loop_timeout_ms);

    void Start();

    void AddConnection(int connfd, InetAddress &remote_addr);

    size_t ConnectionCount();

    void RunFunction(const EventLoop::Functor &func);

   private:
    void ThreadFunc();

    void DisconnectedCallback(TcpConnectionSptr tcp_conn_sptr, misc::SimpleTimeSptr happend_st_sptr);

    int id_;

    TcpServer *server_ptr_;

    std::unique_ptr<std::thread> thread_uptr_;
    EventLoopUptr loop_uptr_;
    std::map<int, TcpConnectionSptr> conn_map_;
  };

  misc::ErrorUptr ListenAndServe();

  void ListenCallback(misc::SimpleTimeSptr happened_st_sptr);

  std::string ip_;
  uint16_t port_;
  log::LoggerSptr logger_sptr_;

  int listenfd_;
  EventLoopUptr loop_uptr_;
  size_t dispatch_index_;

  std::vector<std::unique_ptr<ConnectionThread>> conn_thread_list_;

  TcpConnCallback connected_callback_;
  TcpConnCallback disconnected_callback_;
  TcpConnCallback read_callback_;
  TcpConnCallback write_complete_callback_;
  TcpConnCallback error_callback_;
};

using TcpServerUptr = std::unique_ptr<TcpServer>;


}

}


#endif //CPPBOX_NET_TCP_SERVER_H
