//
// Created by ligang on 18-11-23.
//

#include "file_writer.h"

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include "cppbox/misc/misc.h"


namespace cppbox {

namespace log {

FileWriter::FileWriter(const std::string &path, size_t bufsize) :
        path_(path),
        fd_(OpenFile(path_.c_str())),
        now_time_uptr_(misc::NowTimeUptr()),
        last_write_seconds_(now_time_uptr_->Sec()),
        buffer_uptr_(new misc::SimpleBuffer(bufsize)) {}

FileWriter::~FileWriter() {
  if (buffer_uptr_->Readable() > 0) {
    WriteUnlocked(buffer_uptr_->ReadBegin(), buffer_uptr_->Readable());
  }

  ::close(fd_);
}

size_t FileWriter::Write(const char *msg, size_t len) {
  std::lock_guard<std::mutex> lock(mutex_);

  EnsureFileExist();

  if (buffer_uptr_->Size() == 0) {
    return WriteUnlocked(msg, len);
  }

  if (len <= buffer_uptr_->Writeable()) {
    buffer_uptr_->Append(msg, len);

    return len;
  }

  FlushUnlocked();
  if (len <= buffer_uptr_->Writeable()) {
    buffer_uptr_->Append(msg, len);

    return len;
  }

  return WriteUnlocked(msg, len);
}

size_t FileWriter::Write(const std::string &msg) {
  return Write(msg.c_str(), msg.size());
}

size_t FileWriter::Flush() {
  if (buffer_uptr_->Size() == 0) {
    return 0;
  }

  std::lock_guard<std::mutex> lock(mutex_);

  EnsureFileExist();

  return FlushUnlocked();
}


int FileWriter::OpenFile(const char *path) {
  return ::open(path, O_WRONLY | O_APPEND | O_CLOEXEC | O_CREAT, 0644);
}

size_t FileWriter::FlushUnlocked() {
  if (buffer_uptr_->Readable() == 0) {
    return 0;
  }

  auto n = WriteUnlocked(buffer_uptr_->ReadBegin(), buffer_uptr_->Readable());
  buffer_uptr_->Reset();

  return n;
}

size_t FileWriter::WriteUnlocked(const char *msg, size_t len) {
  return ::write(fd_, msg, len);
}

void FileWriter::EnsureFileExist() {
  now_time_uptr_->Update();

  if (last_write_seconds_ != now_time_uptr_->Sec()) {
    if (!misc::FileExist(path_.c_str())) {
      ::close(fd_);
      fd_ = OpenFile(path_.c_str());
    }
    last_write_seconds_ = now_time_uptr_->Sec();
  }
}

}


}