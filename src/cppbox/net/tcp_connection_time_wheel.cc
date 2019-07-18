//
// Created by ligang on 19-4-22.
//

#include "tcp_connection_time_wheel.h"

namespace cppbox {

namespace net {


TcpConnectionTimeWheel::TcpConnectionTimeWheel(EventLoop *loop_ptr) :
        hand_(0),
        wheel_(kWheelSize),
        loop_ptr_(loop_ptr),
        time_event_sptr_(new TimeEvent()) {}

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

void TcpConnectionTimeWheel::AddConnection(const TcpConnectionSptr &tcp_conn_sptr, uint16_t timeout_seconds) {
  auto hand   = CalHand(timeout_seconds);
  auto connfd = tcp_conn_sptr->connfd();

  conn_hand_map_.emplace(connfd, hand);
  wheel_[hand].emplace(connfd, tcp_conn_sptr);
}

TcpConnectionSptr TcpConnectionTimeWheel::GetConnection(int connfd) {
  auto hit = conn_hand_map_.find(connfd);
  if (hit == conn_hand_map_.end()) {
    return nullptr;
  }

  auto hand = hit->second;
  auto tit  = wheel_[hand].find(connfd);
  if (tit == wheel_[hand].end()) {
    conn_hand_map_.erase(hit);
    return nullptr;
  }

  return tit->second;
}

void TcpConnectionTimeWheel::DelConnection(int connfd) {
  auto hit = conn_hand_map_.find(connfd);
  if (hit == conn_hand_map_.end()) {
    return;
  }

  auto hand = hit->second;
  conn_hand_map_.erase(hit);

  auto tit = wheel_[hand].find(connfd);
  if (tit != wheel_[hand].end()) {
    wheel_[hand].erase(tit);
  }
}

void TcpConnectionTimeWheel::UpdateConnection(int connfd, uint16_t timeout_seconds) {
  auto hit = conn_hand_map_.find(connfd);
  if (hit == conn_hand_map_.end()) {
    return;
  }

  auto ohand = hit->second;
  auto tit   = wheel_[ohand].find(connfd);
  if (tit == wheel_[ohand].end()) {
    conn_hand_map_.erase(hit);
    return;
  }

  auto nhand = CalHand(timeout_seconds);
  if (nhand != ohand) {
    wheel_[nhand].emplace(connfd, tit->second);
    wheel_[ohand].erase(tit);
    hit->second = nhand;
  }
}


void TcpConnectionTimeWheel::TimeRollFunc(const misc::SimpleTimeSptr &happen_st_sptr) {
  hand_ = (hand_ + 1) % kWheelSize;

  if (wheel_[hand_].empty()) {
    return;
  }

  std::map<int, TcpConnectionSptr> tm;
  wheel_[hand_].swap(tm);

  for (auto &tit : tm) {
    tit.second->TimeoutCallback(happen_st_sptr);
    auto hit = conn_hand_map_.find(tit.first);
    if (hit != conn_hand_map_.end()) {
      conn_hand_map_.erase(hit);
    }
  }
}

uint16_t TcpConnectionTimeWheel::CalHand(uint16_t timeout_seconds) {
  return (hand_ + timeout_seconds + 1) % kWheelSize;
}


}

}