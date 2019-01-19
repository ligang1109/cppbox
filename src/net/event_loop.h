//
// Created by ligang on 19-1-4.
//

#ifndef CPPBOX_NET_EVENT_LOOP_H
#define CPPBOX_NET_EVENT_LOOP_H

#include <map>
#include <atomic>

#include "event.h"

#include "log/base.h"

namespace cppbox {

namespace net {


class EventLoop : public misc::NonCopyable {
 public:
  explicit EventLoop(const log::LoggerSptr &error_logger_sptr = nullptr);

  ~EventLoop();

  misc::ErrorUptr Init();

  void UpdateEvent(EventSptr event_sptr);

  EventSptr GetEvent(int fd);

  void DelEvent(int fd);

  void Loop();

  void Wakeup(uint64_t u);

  void Quit();

 private:
  static const uint64_t kWakeupQuit = 1;

  void WakeupCallback(misc::SimpleTimeSptr happened_st_sptr);

  void HandleEvent(Event *event_p, uint32_t ready_events, misc::SimpleTimeSptr st_sptr);

  int       wakeup_fd_;
  EpollUptr epoll_uptr_;

  std::atomic<bool>        quit_;
  std::map<int, EventSptr> event_map_;

  log::LoggerSptr error_logger_sptr_;
};

using EventLoopUptr = std::unique_ptr<EventLoop>;


}

}


#endif //CPPBOX_NET_EVENT_LOOP_H
