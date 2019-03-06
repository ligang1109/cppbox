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


int TcpConnectionThreadId();

class TcpServer : public misc::NonCopyable {
 public:
  using NewConnectionFunc = std::function<TcpConnectionSptr(int, const InetAddress &, EventLoop *)>;

  explicit TcpServer(uint16_t port, const log::LoggerSptr &logger_sptr = nullptr, const std::string &ip = "127.0.0.1");

  void set_new_conn_func(const NewConnectionFunc &func);

  void set_connected_callback(const TcpConnCallback &cb);

  void set_disconnected_callback(const TcpConnCallback &cb);

  void set_read_callback(const TcpConnCallback &cb);

  void set_write_complete_callback(const TcpConnCallback &cb);

  void set_error_callback(const TcpConnCallback &cb);

  misc::ErrorUptr Init(int conn_thread_cnt, int conn_thread_loop_timeout_ms, size_t conn_idle_seconds = 0, size_t check_idle_interval_seconds = 3, int init_evlist_size = 1024);

  misc::ErrorUptr Start();

  size_t ConnectionCount();

  void RunFunctionInConnectionThread(int conn_thread_id, const EventLoop::Functor &func);

  void RunAtTimeInConnectionThread(time_t abs_sec, const Event::EventCallback &cb);

  void RunAfterTimeInConnectionThread(time_t delay_sec, const Event::EventCallback &cb);

  void RunEveryTimeInConnectionThread(time_t interval_sec, const Event::EventCallback &cb);

 private:
  class ConnectionThread {
   public:
    explicit ConnectionThread(int id, TcpServer *server_ptr, size_t time_wheel_size, size_t check_idle_interval_seconds);

    misc::ErrorUptr Init(int loop_timeout_ms, int init_evlist_size = 1024);

    void Start();

    void AddConnection(int connfd, const InetAddress &remote_addr, const misc::SimpleTimeSptr &happened_st_sptr);

    size_t ConnectionCount();

    void RunFunction(const EventLoop::Functor &func);

    void RunAtTime(time_t abs_sec, const Event::EventCallback &cb);

    void RunAfterTime(time_t delay_sec, const Event::EventCallback &cb);

    void RunEveryTime(time_t interval_sec, const Event::EventCallback &cb);

   private:
    void TimeWheelFunc(const misc::SimpleTimeSptr &happened_st_sptr);

    void DelConnection(int connfd);

    void ConnectionDestructCallback(TcpConnection &tcp_conn);

    void UpdateActiveConnection(const TcpConnectionSptr &tcp_conn_sptr);

    void ConnectionReadCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happened_st_sptr);

    void ThreadFunc();

    void AddConnectionInThread(int connfd, const InetAddress &remote_addr, const misc::SimpleTimeSptr &happened_st_sptr);

    void DisconnectedCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happened_st_sptr);

    void EnsureAddTimeEvent();

    int    id_;
    size_t check_idle_interval_seconds_;

    TcpServer                    *server_ptr_;
    std::unique_ptr<std::thread> thread_uptr_;

    EventLoopUptr loop_uptr_;
    TimeEventSptr time_event_sptr_;
    bool          has_added_time_event_;

    std::vector<std::map<int, TcpConnectionSptr>> time_wheel_;
    size_t                                        time_hand_;
    std::map<int, size_t>                         conn_time_wheel_map_;
    TimeEventSptr                                 time_wheel_event_sptr_;
  };

  misc::ErrorUptr ListenAndServe();

  void ListenCallback(const misc::SimpleTimeSptr &happened_st_sptr);

  TcpConnectionSptr DefaultNewConnection(int connfd, const InetAddress &remote_addr, EventLoop *loop_ptr);

  std::string     ip_;
  uint16_t        port_;
  log::LoggerSptr logger_sptr_;

  int           listenfd_;
  EventLoopUptr loop_uptr_;
  size_t        dispatch_index_;

  std::vector<std::unique_ptr<ConnectionThread>> conn_thread_list_;

  NewConnectionFunc new_conn_func_;

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
