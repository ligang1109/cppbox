//
// Created by ligang on 19-1-4.
//

#include "event_loop.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <iostream>

#include "misc/misc.h"

namespace cppbox {

namespace net {


EventLoop::EventLoop(const log::LoggerSptr &error_logger_sptr) :
        quit_(false),
        error_logger_sptr_(error_logger_sptr) {
  if (error_logger_sptr_ == nullptr) {
    error_logger_sptr_.reset(new log::NullLogger());
  }
}

EventLoop::~EventLoop() {
  ::close(wakeup_fd_);
}

misc::ErrorUptr EventLoop::Init() {
  epoll_uptr_ = misc::MakeUnique<Epoll>();
  auto eu     = epoll_uptr_->Init();
  if (eu != nullptr) {
    return eu;
  }

  wakeup_fd_ = ::eventfd(0, EFD_CLOEXEC);
  if (wakeup_fd_ == -1) {
    return misc::NewErrorUptrByErrno();
  }

  auto event_sptr = std::make_shared<Event>(wakeup_fd_);
  event_sptr->set_events(Event::kReadEvents);
  event_sptr->set_read_callback(std::bind(&EventLoop::WakeupCallback, this, std::placeholders::_1));

  UpdateEvent(event_sptr);

  return nullptr;
}

void EventLoop::UpdateEvent(EventSptr event_sptr) {
  if (event_map_.find(event_sptr->fd()) == event_map_.end()) {
    epoll_uptr_->Add(event_sptr->fd(), event_sptr->events());
  } else {
    epoll_uptr_->Mod(event_sptr->fd(), event_sptr->events());
  }

  event_map_[event_sptr->fd()] = event_sptr;
}

void EventLoop::DelEvent(int eventfd) {
  epoll_uptr_->Del(eventfd, 0);
  event_map_.erase(eventfd);
}

void EventLoop::Loop() {
  quit_ = false;

  while (!quit_) {
    Epoll::ReadyList ready_list;

    auto eu = epoll_uptr_->Wait(&ready_list, -1);
    if (eu != nullptr) {
      error_logger_sptr_->Error("epoll wait error: " + eu->String());
      return;
    }

    auto st_sptr = misc::NowTimeSptr();

    for (auto &ready :ready_list) {
      auto it = event_map_.find(ready.first);
      if (it != event_map_.end()) {
        HandleEvent(it->second.get(), ready.second, st_sptr);
      }
    }
  }
}

void EventLoop::Wakeup(uint64_t u) {
  ::write(wakeup_fd_, &u, sizeof(uint64_t));
}

void EventLoop::Quit() {
  Wakeup(kWakeupQuit);
}

void EventLoop::WakeupCallback(misc::SimpleTimeSptr happened_st_sptr) {
  uint64_t u;
  ::read(wakeup_fd_, &u, sizeof(uint64_t));

  switch (u) {
    case kWakeupQuit:
      quit_ = true;
      break;
//    default:

  }
}

void EventLoop::HandleEvent(Event *event_p, uint32_t ready_events, misc::SimpleTimeSptr st_sptr) {
  if (ready_events & Event::kErrorEvents) {
    auto ecb = event_p->error_callback();
    if (ecb) {
      ecb(st_sptr);
      return;
    }
  }

  if (ready_events & Event::kReadEvents) {
    auto rcb = event_p->read_callback();
    if (rcb) {
      rcb(st_sptr);
    }
  }

  if (ready_events & Event::kWriteEvents) {
    auto wcb = event_p->write_callback();
    if (wcb) {
      wcb(st_sptr);
    }
  }
}


}

}