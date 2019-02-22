//
// Created by ligang on 19-1-8.
//

#ifndef CPPBOX_MISC_SIMPLE_TIME_H
#define CPPBOX_MISC_SIMPLE_TIME_H

#include <sys/time.h>

#include "error.h"

namespace cppbox {

namespace misc {

static const char *kGeneralTimeLayout1 = "%Y-%m-%d %H:%M:%S";

class SimpleTime {
 public:
  explicit SimpleTime(time_t tv_sec, suseconds_t tv_usec = 0);

  ErrorUptr ParseTime(const char *time_str, const char *layout = kGeneralTimeLayout1);

  time_t Sec();

  suseconds_t Usec();

  std::string Format(const char *layout = kGeneralTimeLayout1);

  void Update();

  void Add(time_t tv_sec);

  void Sub(time_t tv_sec);

 private:
  struct timeval tv_;
};

using SimpleTimeUptr = std::unique_ptr<SimpleTime>;
using SimpleTimeSptr = std::shared_ptr<SimpleTime>;


SimpleTimeUptr NowTimeUptr();

SimpleTimeSptr NowTimeSptr();


}

}

#endif //CPPBOX_MISC_SIMPLE_TIME_H
