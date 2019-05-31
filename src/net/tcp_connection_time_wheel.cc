//
// Created by ligang on 19-4-22.
//

#include "tcp_connection_time_wheel.h"

namespace cppbox {

namespace net {


TcpConnectionTimeWheel::TcpConnectionTimeWheel(EventLoop *loop_ptr, const TcpConnectionCallback &timeout_callback) :
        hand_(0),
        wheel_(kWheelSize),
        loop_ptr_(loop_ptr),
        time_event_sptr_(new TimeEvent()),
        timeout_callback_(timeout_callback) {}

TcpConnectionTimeWheel::~TcpConnectionTimeWheel() {
  auto fd = time_event_sptr_->fd();
  if (fd > 0) {
    loop_ptr_->DelEvent(fd);
  }
}

misc::ErrorUptr TcpConnectionTimeWheel::Init() {
  auto err_uptr = time_event_sptr_->Init();
  if (err_uptr != nullptr) {
    return err_uptr;
  }

  time_event_sptr_->RunEvery(1, std::bind(&TcpConnectionTimeWheel::TimeRollFunc, this, std::placeholders::_1));
  loop_ptr_->UpdateEvent(time_event_sptr_);

  return nullptr;
}

uint16_t TcpConnectionTimeWheel::AddConnection(const TcpConnectionSptr &tcp_conn_sptr, uint16_t timeout_seconds) {
  auto hand = CalHand(timeout_seconds);

  wheel_[hand].emplace(tcp_conn_sptr->connfd(), tcp_conn_sptr);

  return hand;
}

TcpConnectionSptr TcpConnectionTimeWheel::GetConnection(uint16_t hand, int connfd) {
  auto it = wheel_[hand].find(connfd);
  if (it != wheel_[hand].end()) {
    return it->second;
  }

  return nullptr;
}

void TcpConnectionTimeWheel::DelConnection(uint16_t hand, int connfd) {
  auto it = wheel_[hand].find(connfd);
  if (it != wheel_[hand].end()) {
    wheel_[hand].erase(it);
  }
}

size_t TcpConnectionTimeWheel::UpdateConnection(uint16_t hand, int connfd, uint16_t timeout_seconds) {
  auto it = wheel_[hand].find(connfd);
  if (it == wheel_[hand].end()) {
    return kWheelSize;
  }

  auto nhand = CalHand(timeout_seconds);
  if (nhand != hand) {
    wheel_[nhand].emplace(connfd, it->second);
    wheel_[hand].erase(it);
  }

  return nhand;
}


void TcpConnectionTimeWheel::TimeRollFunc(const misc::SimpleTimeSptr &happen_st_sptr) {
  hand_ = (hand_ + 1) % kWheelSize;

  if (wheel_[hand_].empty()) {
    return;
  }

  std::map<int, TcpConnectionSptr> tm;
  wheel_[hand_].swap(tm);

  for (auto &it : tm) {
    timeout_callback_(it.second, happen_st_sptr);
  }
}

uint16_t TcpConnectionTimeWheel::CalHand(uint16_t timeout_seconds) {
  return (hand_ + timeout_seconds + 1) % kWheelSize;
}


}

}