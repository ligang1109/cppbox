//
// Created by ligang on 18-11-28.
//

#ifndef CPPBOX_LOG_SIMPLE_FORMATER_H
#define CPPBOX_LOG_SIMPLE_FORMATER_H

#include "base.h"

namespace cppbox {

class SimpleFormater : public FormaterInterface {
 public:
  explicit SimpleFormater(const std::string &log_id = "-", const std::string &address = "-");

  std::string Format(LogLevel level, const std::string &msg) override;

 private:
  std::string log_id_;
  std::string address_;

  struct timeval last_fmt_time_;
  time_t         last_fmt_seconds_;
  std::string    last_fmt_seconds_str_;
};

}


#endif //CPPBOX_LOG_SIMPLE_FORMATER_H
