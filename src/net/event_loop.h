//
// Created by ligang on 19-1-4.
//

#ifndef CPPBOX_NET_EVENT_LOOP_H
#define CPPBOX_NET_EVENT_LOOP_H

#include <map>
#include <atomic>
#include <mutex>

#include "event.h"

#include "log/base.h"

namespace cppbox {

namespace net {


class EventLoop : public misc::NonCopyable {
 public:
  using Functor = std::function<void()>;

  explicit EventLoop(const log::LoggerSptr &logger_sptr = nullptr, int timeout_ms = -1);

  ~EventLoop();

  misc::ErrorUptr Init();

  void UpdateEvent(EventSptr event_sptr);

  EventSptr GetEvent(int fd);

  void DelEvent(int fd);

  void Loop();

  void Quit();

  void AppendFunction(const Functor &func);

 private:
  void Wakeup();

  void WakeupCallback(misc::SimpleTimeSptr happened_st_sptr);

  void HandleEvent(Event *event_p, uint32_t ready_events, misc::SimpleTimeSptr st_sptr);

  void RunFunctions();

  int wakeup_fd_;
  EpollUptr epoll_uptr_;

  std::atomic<bool> quit_;
  std::map<int, EventSptr> event_map_;

  std::mutex mutex_;
  std::vector<Functor> function_list_;
  bool handling_events_;

  log::LoggerSptr logger_sptr_;
  int timeout_ms_;
};

using EventLoopUptr = std::unique_ptr<EventLoop>;


}

}


#endif //CPPBOX_NET_EVENT_LOOP_H
