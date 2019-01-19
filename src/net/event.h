//
// Created by ligang on 19-1-15.
//

#ifndef CPPBOX_NET_EVENT_H
#define CPPBOX_NET_EVENT_H


#include <functional>

#include "epoll.h"

#include "misc/non_copyable.h"
#include "misc/simple_time.h"

namespace cppbox {

namespace net {


class Event : public misc::NonCopyable {
 public:
  using EventCallback = std::function<void(misc::SimpleTimeSptr)>;

  static const uint32_t kReadEvents  = EPOLLIN | EPOLLPRI;
  static const uint32_t kWriteEvents = EPOLLOUT;
  static const uint32_t kErrorEvents = EPOLLERR;

  Event();

  explicit Event(int fd);

  ~Event() = default;

  int fd();

  uint32_t events();

  void set_events(uint32_t events);

  EventCallback read_callback();

  void set_read_callback(EventCallback cb);

  EventCallback write_callback();

  void set_write_callback(EventCallback cb);

  EventCallback error_callback();

  void set_error_callback(EventCallback cb);

  void AddEvents(uint32_t events);

  void DelEvents(uint32_t events);

  bool HasEvents(uint32_t events);

 protected:
  int      fd_;
  uint32_t events_;

  EventCallback read_callback_;
  EventCallback write_callback_;
  EventCallback error_callback_;
};

using EventUptr = std::unique_ptr<Event>;
using EventSptr = std::shared_ptr<Event>;


class TimeEvent : public Event {
 public:
  ~TimeEvent();

  misc::ErrorUptr Init();

  void RunAt(time_t abs_sec, EventCallback cb);

  void RunAfter(time_t delay_sec, EventCallback cb);

  void RunEvery(time_t interval_sec, EventCallback cb);

 private:
  void TimeUpCallback(misc::SimpleTimeSptr happened_st_sptr);

  EventCallback time_callback_;
};


}

}


#endif //CPPBOX_NET_EVENT_H