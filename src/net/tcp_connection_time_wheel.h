//
// Created by ligang on 19-4-22.
//

#ifndef CPPBOX_NET_TCP_CONN_TIME_WHEEL_H
#define CPPBOX_NET_TCP_CONN_TIME_WHEEL_H

#include "tcp_connection.h"

namespace cppbox {

namespace net {


class TcpConnectionTimeWheel {
 public:
  static const uint16_t kMaxConnIdleSeconds = 65535;
  static const size_t   kWheelSize          = 65536;

  explicit TcpConnectionTimeWheel(EventLoop *loop_ptr, const TcpConnectionCallback &timeout_callback);

  ~TcpConnectionTimeWheel();

  misc::ErrorUptr Init();

  uint16_t AddConnection(const TcpConnectionSptr &tcp_conn_sptr, uint16_t timeout_seconds);

  TcpConnectionSptr GetConnection(uint16_t hand, int connfd);

  void DelConnection(uint16_t hand, int connfd);

  size_t UpdateConnection(uint16_t hand, int connfd, uint16_t timeout_seconds);

 private:
  void TimeRollFunc(const misc::SimpleTimeSptr &happen_st_sptr);

  uint16_t CalHand(uint16_t timeout_seconds);

  uint16_t                                      hand_;
  std::vector<std::map<int, TcpConnectionSptr>> wheel_;

  EventLoop     *loop_ptr_;
  TimeEventSptr time_event_sptr_;

  TcpConnectionCallback timeout_callback_;
};

using TcpConnectionTimeWheelUptr = std::unique_ptr<TcpConnectionTimeWheel>;


}

}


#endif //CPPBOX_NET_TCP_CONN_TIME_WHEEL_H
