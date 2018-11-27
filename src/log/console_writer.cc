//
// Created by ligang on 18-11-27.
//

#include "console_writer.h"

#include <iostream>

namespace cppbox {

size_t ConsoleWriter::Write(const std::string &msg) {
  std::lock_guard<std::mutex> lock(mutex_);

  std::cout << msg << std::flush;
}

size_t ConsoleWriter::Write(const char *msg, size_t len) {
  std::string str(msg, len);

  Write(str);
}

}