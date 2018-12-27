//
// Created by ligang on 18-11-23.
//

#ifndef CPPBOX_LOG_FILE_WRITER_H
#define CPPBOX_LOG_FILE_WRITER_H

#include <mutex>

#include "base.h"

#include "misc/non_copyable.h"

namespace cppbox {

namespace log {


class FileWriter : public WriterInterface, public misc::NonCopyable {
 public:
  explicit FileWriter(const char *path);

  ~FileWriter() override;

  size_t Write(const char *msg, size_t len) override;

  size_t Write(const std::string &msg) override;

  int Flush() override;

 private:
  const char *path_;
  FILE       *fp_;

  struct timeval now_time_;
  time_t         last_write_seconds_;

  std::mutex mutex_;

  int FlushUnlocked();
};


}


}

#endif //CPPBOX_LOG_FILE_WRITER_H
