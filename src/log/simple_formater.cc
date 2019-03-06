//
// Created by ligang on 18-11-28.
//

#include "simple_formater.h"

#include <sys/time.h>

#include "base.h"

#include "misc/misc.h"

namespace cppbox {

namespace log {


SimpleFormater::SimpleFormater(const std::string &log_id, const std::string &address) :
        log_id_(log_id),
        address_(address),
        last_fmt_time_uptr_(misc::NowTimeUptr()),
        last_fmt_seconds_(last_fmt_time_uptr_->Sec()),
        last_fmt_seconds_str_(last_fmt_time_uptr_->Format()) {}

std::string SimpleFormater::Format(LogLevel level, const std::string &msg) {
  last_fmt_time_uptr_->Update();

  {
    std::lock_guard<std::mutex> lock(mutex_);

    if (last_fmt_seconds_ != last_fmt_time_uptr_->Sec()) {
      last_fmt_seconds_     = last_fmt_time_uptr_->Sec();
      last_fmt_seconds_str_ = last_fmt_time_uptr_->Format();
    }
  }

  return kLogLevelMsgList[static_cast<int>(level)] + "\t" +
         log_id_ + "\t" +
         address_ + "\t" +
         "[" + last_fmt_seconds_str_ + "." + std::to_string(last_fmt_time_uptr_->Usec()) + "]\t" +
         msg + "\n";
}


}


}