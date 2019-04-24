//
// Created by ligang on 18-11-28.
//

#include "simple_formater.h"

#include <sys/time.h>

#include "base.h"

#include "misc/misc.h"

namespace cppbox {

namespace log {


SimpleFormater::SimpleFormater(const char *time_layout) :
        time_layout_(time_layout),
        last_fmt_time_uptr_(misc::NowTimeUptr()),
        last_fmt_seconds_(last_fmt_time_uptr_->Sec()),
        last_fmt_seconds_str_(last_fmt_time_uptr_->Format(time_layout_.c_str())) {}

std::string SimpleFormater::Format(LogLevel level, const std::string &msg) {
  {
    std::lock_guard<std::mutex> lock(mutex_);

    last_fmt_time_uptr_->Update();

    if (last_fmt_seconds_ != last_fmt_time_uptr_->Sec()) {
      last_fmt_seconds_ = last_fmt_time_uptr_->Sec();
      last_fmt_seconds_str_ = last_fmt_time_uptr_->Format(time_layout_.c_str());
    }
  }

  return kLogLevelMsgList[static_cast<int>(level)] + "\t" +
         "[" + last_fmt_seconds_str_ + "]\t" +
         msg + "\n";
}


}


}