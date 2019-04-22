//
// Created by ligang on 19-4-22.
//

#include "tcp_conn_time_wheel.h"

namespace cppbox {

namespace net {

TcpConnTimeWheel::TcpConnTimeWheel(EventLoop *loop_ptr) :
        hand_(0),
        wheel_(kWheelSize),
        loop_ptr_(loop_ptr),
        time_event_sptr_(new TimeEvent()) {}

TcpConnTimeWheel::~TcpConnTimeWheel() {
  loop_ptr_->DelEvent(time_event_sptr_->fd());
}

misc::ErrorUptr TcpConnTimeWheel::Init() {
  auto err_uptr = time_event_sptr_->Init();
  if (err_uptr != nullptr) {
    return err_uptr;
  }

  time_event_sptr_->RunEvery(1, std::bind(&TcpConnTimeWheel::TimeRollFunc, this, std::placeholders::_1));
  loop_ptr_->UpdateEvent(time_event_sptr_);

  return nullptr;
}

uint16_t TcpConnTimeWheel::AddConnection(const TcpConnectionSptr &tcp_conn_sptr, uint16_t timeout_seconds) {
  auto hand = CalHand(timeout_seconds);

  wheel_[hand][tcp_conn_sptr->connfd()] = tcp_conn_sptr;

  return hand;
}

TcpConnectionSptr TcpConnTimeWheel::GetConnection(uint16_t hand, int connfd) {
  auto it = wheel_[hand].find(connfd);
  if (it != wheel_[hand].end()) {
    return it->second;
  }

  return nullptr;
}

void TcpConnTimeWheel::DelConnection(uint16_t hand, int connfd) {
  wheel_[hand].erase(connfd);
}

size_t TcpConnTimeWheel::UpdateConnection(uint16_t hand, int connfd, uint16_t timeout_seconds) {
  auto it = wheel_[hand].find(connfd);
  if (it == wheel_[hand].end()) {
    return kWheelSize;
  }

  auto nhand = CalHand(timeout_seconds);
  if (nhand != hand) {
    wheel_[nhand][connfd] = it->second;
    wheel_[hand].erase(connfd);
  }

  return nhand;
}


void TcpConnTimeWheel::TimeRollFunc(const misc::SimpleTimeSptr &happened_st_sptr) {
  hand_ = (hand_ + 1) % kWheelSize;

  if (!wheel_[hand_].empty()) {
    wheel_[hand_].clear();
  }
}

uint16_t TcpConnTimeWheel::CalHand(uint16_t timeout_seconds) {
  return (hand_ + timeout_seconds + 1) % kWheelSize;
}

}

}