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

void Event::set_fd(int fd) {
  fd_ = fd;
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

void Event::set_read_callback(const EventCallback &cb) {
  read_callback_ = cb;
}

Event::EventCallback Event::write_callback() {
  return write_callback_;
}

void Event::set_write_callback(const EventCallback &cb) {
  write_callback_ = cb;
}

Event::EventCallback Event::error_callback() {
  return error_callback_;
}

void Event::set_error_callback(const EventCallback &cb) {
  error_callback_ = cb;
}

void Event::AddEvents(uint32_t events) {
  events_ |= events;
}

void Event::DelEvents(uint32_t events) {
  events_ &= ~events;
}

bool Event::HasEvents(uint32_t events) {
  return (events_ & events) == events;
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


void TimeEvent::RunAt(time_t abs_sec, const EventCallback &cb) {
  struct itimerspec new_value;
  memset(&new_value, 0, sizeof(struct itimerspec));

  new_value.it_value.tv_sec = abs_sec - misc::NowTimeUptr()->Sec();

  ::timerfd_settime(fd_, 0, &new_value, nullptr);

  time_callback_ = cb;
}

void TimeEvent::RunAfter(time_t delay_sec, const EventCallback &cb) {
  struct itimerspec new_value;
  memset(&new_value, 0, sizeof(struct itimerspec));

  new_value.it_value.tv_sec = delay_sec;

  ::timerfd_settime(fd_, 0, &new_value, nullptr);

  time_callback_ = cb;
}

void TimeEvent::RunEvery(time_t interval_sec, const EventCallback &cb) {
  struct itimerspec new_value;
  memset(&new_value, 0, sizeof(struct itimerspec));

  new_value.it_interval.tv_sec = interval_sec;
  new_value.it_value.tv_sec    = interval_sec;

  ::timerfd_settime(fd_, 0, &new_value, nullptr);

  time_callback_ = cb;
}

void TimeEvent::TimeUpCallback(const misc::SimpleTimeSptr &happen_st_sptr) {
  uint64_t u;
  ::read(fd_, &u, sizeof(uint64_t));

  time_callback_(happen_st_sptr);
}

}

}