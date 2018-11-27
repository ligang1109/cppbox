//
// Created by ligang on 18-11-28.
//

#include "simple_formater.h"

#include <sys/time.h>

#include "base.h"

#include "misc/misc.h"

namespace cppbox {

SimpleFormater::SimpleFormater(const std::string &log_id, const std::string &address) :
        log_id_(log_id),
        address_(address) {
  gettimeofday(&last_fmt_time_, nullptr);
  last_fmt_seconds_     = last_fmt_time_.tv_sec;
  last_fmt_seconds_str_ = Misc::FormatTime(last_fmt_seconds_);
}

std::string SimpleFormater::Format(cppbox::LogLevel level, const std::string &msg) {
  gettimeofday(&last_fmt_time_, nullptr);
  if (last_fmt_seconds_ != last_fmt_time_.tv_sec) {
    last_fmt_seconds_     = last_fmt_time_.tv_sec;
    last_fmt_seconds_str_ = Misc::FormatTime(last_fmt_seconds_);
  }

  return kLogLevelMsgList[static_cast<int>(level)] + "\t" +
         log_id_ + "\t" +
         address_ + "\t" +
         "[" + last_fmt_seconds_str_ + "." + std::to_string(last_fmt_time_.tv_usec) + "]\t" +
         msg + "\n";
}

}