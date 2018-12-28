//
// Created by ligang on 19-1-15.
//

#include "event.h"

#include <sys/timerfd.h>
#include <unistd.h>


namespace cppbox {

namespace net {

Event::Event() :
        fd_(0),
        events_(0) {}

Event::Event(int fd) :
        fd_(fd),
        events_(0) {}

int Event::fd() {
  return fd_;
}

uint32_t Event::events() {
  return events_;
}

void Event::set_events(uint32_t events) {
  events_ = events;
}

Event::EventCallback Event::read_callback() {
  return read_callback_;
}

void Event::set_read_callback(EventCallback cb) {
  read_callback_ = std::move(cb);
}

Event::EventCallback Event::write_callback() {
  return write_callback_;
}

void Event::set_write_callback(EventCallback cb) {
  write_callback_ = std::move(cb);
}

Event::EventCallback Event::error_callback() {
  return error_callback_;
}

void Event::set_error_callback(EventCallback cb) {
  error_callback_ = std::move(cb);
}


TimeEvent::~TimeEvent() {
  ::close(fd_);
}

misc::ErrorUptr TimeEvent::Init() {
  fd_ = ::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
  if (fd_ == -1) {
    return misc::NewErrorUptrByErrno();
  }

  events_        = kReadEvents;
  read_callback_ = std::bind(&TimeEvent::TimeUpCallback, this, std::placeholders::_1);

  return nullptr;
}


void TimeEvent::RunAt(time_t abs_sec, EventCallback cb) {
  struct itimerspec new_value;
  memset(&new_value, 0, sizeof(struct itimerspec));

  new_value.it_value.tv_sec = abs_sec - misc::NowTimeUptr()->Sec();

  ::timerfd_settime(fd_, 0, &new_value, nullptr);

  time_callback_ = std::move(cb);
}

void TimeEvent::RunAfter(time_t delay_sec, EventCallback cb) {
  struct itimerspec new_value;
  memset(&new_value, 0, sizeof(struct itimerspec));

  new_value.it_value.tv_sec = delay_sec;

  ::timerfd_settime(fd_, 0, &new_value, nullptr);

  time_callback_ = std::move(cb);
}

void TimeEvent::RunEvery(time_t interval_sec, EventCallback cb) {
  struct itimerspec new_value;
  memset(&new_value, 0, sizeof(struct itimerspec));

  new_value.it_interval.tv_sec = interval_sec;
  new_value.it_value.tv_sec    = interval_sec;

  ::timerfd_settime(fd_, 0, &new_value, nullptr);

  time_callback_ = std::move(cb);
}

void TimeEvent::TimeUpCallback(misc::SimpleTimeSptr happened_st_sptr) {
  uint64_t u;
  ::read(fd_, &u, sizeof(uint64_t));

  time_callback_(happened_st_sptr);
}

}

}