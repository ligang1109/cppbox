//
// Created by ligang on 19-1-4.
//

#ifndef CPPBOX_NET_EPOLL_H
#define CPPBOX_NET_EPOLL_H

#include <sys/epoll.h>

#include <vector>

#include "misc/error.h"

namespace cppbox {

namespace net {


class Epoll {
 public:
  using ReadyList = std::vector<std::pair<int, uint32_t>>;

  explicit Epoll(int init_evlist_size = 1024);

  ~Epoll();

  misc::ErrorUptr Init();

  misc::ErrorUptr Add(int fd, uint32_t events);

  misc::ErrorUptr Mod(int fd, uint32_t events);

  misc::ErrorUptr Del(int fd);

  misc::ErrorUptr Wait(ReadyList *ready_list, int timeout_ms);

  void PrintEvents(uint32_t events);

 private:
  misc::ErrorUptr Ctl(int fd, int op, uint32_t events);

  int epfd_;

  std::vector<struct epoll_event> evlist_;
};

using EpollUptr = std::unique_ptr<Epoll>;


}

}

#endif //CPPBOX_NET_EPOLL_H
