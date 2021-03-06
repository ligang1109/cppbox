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

  explicit TcpConnectionTimeWheel(EventLoop *loop_ptr);

  ~TcpConnectionTimeWheel();

  misc::ErrorUptr Init();

  void AddConnection(const TcpConnectionSptr &tcp_conn_sptr, uint16_t timeout_seconds);

  TcpConnectionSptr GetConnection(int connfd);

  void DelConnection(int connfd);

  void UpdateConnection(int connfd, uint16_t timeout_seconds);

 private:
  void TimeRollFunc(const misc::SimpleTimeSptr &happen_st_sptr);

  uint16_t CalHand(uint16_t timeout_seconds);

  uint16_t                                      hand_;
  std::map<int, uint16_t>                       conn_hand_map_;
  std::vector<std::map<int, TcpConnectionSptr>> wheel_;

  EventLoop     *loop_ptr_;
  TimeEventSptr time_event_sptr_;
};

using TcpConnectionTimeWheelUptr = std::unique_ptr<TcpConnectionTimeWheel>;


}

}


#endif //CPPBOX_NET_TCP_CONN_TIME_WHEEL_H
