//
// Created by ligang on 18-11-23.
//

#include "file_writer.h"

#include <sys/time.h>

#include "misc/misc.h"


namespace cppbox {

FileWriter:: FileWriter(const char *path) :
        path_(path),
        fp_(fopen(path, "a")) {
  gettimeofday(&now_time_, nullptr);
  last_write_seconds_ = now_time_.tv_sec;
}

FileWriter::~FileWriter() {
  if (fp_ != nullptr) {
    fclose(fp_);
  }
}

size_t FileWriter::Write(const char *msg, size_t len) {
  std::lock_guard<std::mutex> lock(mutex_);

  gettimeofday(&now_time_, nullptr);
  if (last_write_seconds_ != now_time_.tv_sec) {
    if (!Misc::FileExist(path_)) {
      fclose(fp_);
      fp_ = fopen(path_, "a");
    }
    last_write_seconds_ = now_time_.tv_sec;
  }

  return ::fwrite_unlocked(msg, 1, len, fp_);
}

size_t FileWriter::Write(const std::string &msg) {
  return Write(msg.c_str(), msg.size());
}

int FileWriter::Flush() {
  std::lock_guard<std::mutex> lock(mutex_);

  return FlushUnlocked();
}

int FileWriter::FlushUnlocked() {
  return ::fflush(fp_);
}

}