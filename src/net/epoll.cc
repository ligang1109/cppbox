//
// Created by ligang on 19-1-4.
//

#include "epoll.h"

#include <unistd.h>

#include <iostream>


namespace cppbox {

namespace net {


Epoll::Epoll(int init_evlist_size) :
        evlist_(static_cast<size_t >(init_evlist_size)) {}

Epoll::~Epoll() {
  ::close(epfd_);
}

misc::ErrorUptr Epoll::Init() {
  epfd_ = ::epoll_create1(EPOLL_CLOEXEC);
  if (epfd_ == -1) {
    return misc::NewErrorUptrByErrno();
  }

  return nullptr;
}

misc::ErrorUptr Epoll::Add(int fd, uint32_t events) {
  return Ctl(fd, EPOLL_CTL_ADD, events);
}

misc::ErrorUptr Epoll::Mod(int fd, uint32_t events) {
  return Ctl(fd, EPOLL_CTL_MOD, events);
}

misc::ErrorUptr Epoll::Del(int fd, uint32_t events) {
  return Ctl(fd, EPOLL_CTL_DEL, events);
}

misc::ErrorUptr Epoll::Ctl(int fd, int op, uint32_t events) {
  struct epoll_event ev;
  memset(&ev, 0, sizeof ev);
  ev.events = events;
  ev.data.fd = fd;

  if (::epoll_ctl(epfd_, op, fd, &ev) == -1) {
    return misc::NewErrorUptrByErrno();
  }

  return nullptr;
}

misc::ErrorUptr Epoll::Wait(ReadyList *ready_list, int timeout_ms) {
  int n = ::epoll_wait(epfd_, evlist_.begin().base(), static_cast<int>(evlist_.size()), timeout_ms);
  if (n == -1) {
    if (errno == EINTR) {
      return nullptr;
    }
    return misc::NewErrorUptrByErrno();
  }

  if (n == 0) {
    return nullptr;
  }

  for (auto i = 0; i < n; ++i) {
    int fd = evlist_[i].data.fd;
    uint32_t events = evlist_[i].events;
    ready_list->push_back({fd, events});
  }

  if (n == evlist_.size()) {
    evlist_.resize(evlist_.size() * 2);
  }

  return nullptr;
}

void Epoll::PrintEvents(uint32_t events) {
  if (events & EPOLLIN) {
    std::cout << "EPOLLIN" << std::endl;
  }
  if (events & EPOLLPRI) {
    std::cout << "EPOLLPRI" << std::endl;
  }
  if (events & EPOLLRDHUP) {
    std::cout << "EPOLLRDHUP" << std::endl;
  }
  if (events & EPOLLOUT) {
    std::cout << "EPOLLOUT" << std::endl;
  }
  if (events & EPOLLET) {
    std::cout << "EPOLLET" << std::endl;
  }
  if (events & EPOLLONESHOT) {
    std::cout << "EPOLLONESHOT" << std::endl;
  }
  if (events & EPOLLERR) {
    std::cout << "EPOLLERR" << std::endl;
  }
  if (events & EPOLLHUP) {
    std::cout << "EPOLLHUP" << std::endl;
  }

}


}

}