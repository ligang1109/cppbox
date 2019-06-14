//
// Created by ligang on 19-1-4.
//

#ifndef CPPBOX_NET_EVENT_LOOP_H
#define CPPBOX_NET_EVENT_LOOP_H

#include <map>
#include <atomic>
#include <mutex>

#include "event.h"
#include "cppbox/misc/simple_time.h"

namespace cppbox {

namespace net {


class EventLoop : public misc::NonCopyable {
 public:
  using Functor = std::function<void()>;

  explicit EventLoop(int timeout_ms = -1);

  ~EventLoop();

  misc::ErrorUptr Init(int init_evlist_size = 1024);

  void UpdateEvent(const EventSptr &event_sptr);

  EventSptr GetEvent(int fd);

  void DelEvent(int fd);

  misc::ErrorUptr Loop();

  void Quit();

  void AppendFunction(const Functor &func);

 private:
  void Wakeup();

  void WakeupCallback(const misc::SimpleTimeSptr &happen_st_sptr);

  void HandleEvent(const EventSptr &event_sptr, uint32_t ready_events);

  void RunFunctions();

  int       wakeup_fd_;
  EpollUptr epoll_uptr_;

  std::atomic<bool>        quit_;
  std::map<int, EventSptr> event_map_;

  std::mutex           mutex_;
  std::vector<Functor> function_list_;
  bool handling_events_;

  int timeout_ms_;

  misc::SimpleTimeSptr happen_st_sptr_;

};

using EventLoopUptr = std::unique_ptr<EventLoop>;


}

}


#endif //CPPBOX_NET_EVENT_LOOP_H
