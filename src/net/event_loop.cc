//
// Created by ligang on 19-1-4.
//

#include "event_loop.h"

#include <sys/eventfd.h>
#include <unistd.h>

#include "misc/misc.h"

namespace cppbox {

namespace net {


EventLoop::EventLoop(int timeout_ms) :
        quit_(false),
        handling_events_(false),
        timeout_ms_(timeout_ms) {}

EventLoop::~EventLoop() {
  ::close(wakeup_fd_);
}

misc::ErrorUptr EventLoop::Init(int init_evlist_size) {
  epoll_uptr_ = misc::MakeUnique<Epoll>(init_evlist_size);
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

void EventLoop::UpdateEvent(const EventSptr &event_sptr) {
  auto fd = event_sptr->fd();
  auto it = event_map_.find(fd);
  if (it == event_map_.end()) {
    epoll_uptr_->Add(fd, event_sptr->events());
    event_map_.emplace(fd, event_sptr);
    return;
  }

  epoll_uptr_->Mod(fd, event_sptr->events());
  it->second = event_sptr;
}

EventSptr EventLoop::GetEvent(int fd) {
  auto it = event_map_.find(fd);
  if (it == event_map_.end()) {
    return nullptr;
  }

  return it->second;
}

void EventLoop::DelEvent(int fd) {
  epoll_uptr_->Del(fd);
  event_map_.erase(fd);
}

misc::ErrorUptr EventLoop::Loop() {
  quit_ = false;

  while (!quit_) {
    Epoll::ReadyList ready_list;

    auto eu = epoll_uptr_->Wait(&ready_list, timeout_ms_);
    if (eu != nullptr) {
      return eu;
    }

    auto happened_st_sptr = misc::NowTimeSptr();

    handling_events_ = true;
    for (auto &ready :ready_list) {
      auto it = event_map_.find(ready.first);
      if (it != event_map_.end()) {
        HandleEvent(it->second, ready.second, happened_st_sptr);
      }
    }
    handling_events_ = false;

    RunFunctions();
  }

  return nullptr;
}

void EventLoop::Quit() {
  quit_ = true;
  Wakeup();
}

void EventLoop::Wakeup() {
  uint64_t u = 1;
  ::write(wakeup_fd_, &u, sizeof(uint64_t));
}

void EventLoop::AppendFunction(const Functor &func) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    function_list_.push_back(func);
  }

  if (!handling_events_) {
    Wakeup();
  }
}

void EventLoop::WakeupCallback(const misc::SimpleTimeSptr &happened_st_sptr) {
  uint64_t u;
  ::read(wakeup_fd_, &u, sizeof(uint64_t));
}

void EventLoop::HandleEvent(const EventSptr &event_sptr, uint32_t ready_events, const misc::SimpleTimeSptr &happened_st_sptr) {
  if (ready_events & Event::kErrorEvents) {
    auto ecb = event_sptr->error_callback();
    if (ecb) {
      ecb(happened_st_sptr);
      return;
    }
  }

  if (ready_events & Event::kReadEvents) {
    auto rcb = event_sptr->read_callback();
    if (rcb) {
      rcb(happened_st_sptr);
    }
  }

  if (ready_events & Event::kWriteEvents) {
    auto wcb = event_sptr->write_callback();
    if (wcb) {
      wcb(happened_st_sptr);
    }
  }
}

void EventLoop::RunFunctions() {
  std::vector<Functor> function_list;

  {
    std::lock_guard<std::mutex> lock(mutex_);
    if (function_list_.empty()) {
      return;
    }

    function_list.swap(function_list_);
  }

  for (auto &func : function_list) {
    func();
  }
}


}

}