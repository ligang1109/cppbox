//
// Created by ligang on 18-11-23.
//

#ifndef CPPBOX_LOG_FILE_WRITER_H
#define CPPBOX_LOG_FILE_WRITER_H

#include <mutex>

#include "base.h"

#include "misc/non_copyable.h"
#include "misc/simple_time.h"

namespace cppbox {

namespace log {


class FileWriter : public WriterInterface, public misc::NonCopyable {
 public:
  explicit FileWriter(const std::string &path);

  ~FileWriter() override;

  size_t Write(const char *msg, size_t len) override;

  size_t Write(const std::string &msg) override;

  int Flush() override;

 private:
  int FlushUnlocked();

  std::string path_;
  FILE *fp_;

  misc::SimpleTimeUptr now_time_uptr_;
  time_t last_write_seconds_;

  std::mutex mutex_;
};


}


}

#endif //CPPBOX_LOG_FILE_WRITER_H
