//
// Created by ligang on 18-11-27.
//

#ifndef CPPBOX_LOG_CONSOLE_WRITER_H
#define CPPBOX_LOG_CONSOLE_WRITER_H

#include <mutex>

#include "base.h"

#include "misc/non_copyable.h"

namespace cppbox {

class ConsoleWriter : public WriterInterface, public NonCopyable {
 public:
  ~ConsoleWriter() override = default;

  size_t Write(const char *msg, size_t len) override;

  size_t Write(const std::string &msg) override;

  int Flush() override {}

 private:
  std::mutex mutex_;
};

}


#endif //CPPBOX_LOG_CONSOLE_WRITER_H
