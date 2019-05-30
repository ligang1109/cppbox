//
// Created by ligang on 19-1-8.
//

#include "simple_time.h"

#include "string.h"


namespace cppbox {

namespace misc {


SimpleTime::SimpleTime(time_t tv_sec, suseconds_t tv_usec) {
  tv_.tv_sec  = tv_sec;
  tv_.tv_usec = tv_usec;
}

ErrorUptr SimpleTime::ParseTime(const char *time_str, const char *layout) {
  struct tm tmp;
  memset(&tmp, 0, sizeof(struct tm));

  auto r = ::strptime(time_str, layout, &tmp);
  if (r == nullptr) {
    return NewErrorUptr(Error::kInvalidArg, "strptime failed");
  }

  tmp.tm_isdst = -1;
  tv_.tv_sec   = mktime(&tmp);
  tv_.tv_usec  = 0;
}

time_t SimpleTime::Sec() {
  return tv_.tv_sec;
}

suseconds_t SimpleTime::Usec() {
  return tv_.tv_usec;
}

std::string SimpleTime::Format(const char *layout) {
  char      buf[50];
  struct tm tmp;
  localtime_r(&(tv_.tv_sec), &tmp);
  ::strftime(buf, sizeof(buf), layout, &tmp);

  return std::string(buf);
}

void SimpleTime::Update(time_t tv_sec, suseconds_t tv_usec) {
  if (tv_sec == 0 && tv_usec == 0) {
    gettimeofday(&tv_, nullptr);
  } else {
    tv_.tv_sec  = tv_sec;
    tv_.tv_usec = tv_usec;
  }
}

void SimpleTime::Add(time_t tv_sec) {
  tv_.tv_sec += tv_sec;
}

void SimpleTime::Sub(time_t tv_sec) {
  tv_.tv_sec -= tv_sec;
}


SimpleTimeUptr NowTimeUptr() {
  struct timeval tv;
  gettimeofday(&tv, nullptr);

  return std::unique_ptr<SimpleTime>(new SimpleTime(tv.tv_sec, tv.tv_usec));
}

SimpleTimeSptr NowTimeSptr() {
  struct timeval tv;
  gettimeofday(&tv, nullptr);

  return std::make_shared<SimpleTime>(tv.tv_sec, tv.tv_usec);
}

}

}