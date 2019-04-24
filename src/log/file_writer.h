//
// Created by ligang on 18-11-23.
//

#ifndef CPPBOX_LOG_FILE_WRITER_H
#define CPPBOX_LOG_FILE_WRITER_H

#include <mutex>

#include "base.h"

#include "misc/non_copyable.h"
#include "misc/simple_time.h"
#include "misc/simple_buffer.h"

namespace cppbox {

namespace log {


class FileWriter : public WriterInterface, public misc::NonCopyable {
 public:
  explicit FileWriter(const std::string &path, size_t bufsize = 4096);

  ~FileWriter() override;

  size_t Write(const char *msg, size_t len) override;

  size_t Write(const std::string &msg) override;

  size_t Flush() override;

 private:
  int OpenFile(const char *path);
  
  size_t FlushUnlocked();

  size_t WriteUnlocked(const char *msg, size_t len);

  void EnsureFileExist();

  std::string path_;
  int fd_;

  misc::SimpleTimeUptr now_time_uptr_;
  time_t last_write_seconds_;

  std::mutex mutex_;

  misc::SimpleBufferUptr buffer_uptr_;
};


}


}

#endif //CPPBOX_LOG_FILE_WRITER_H
