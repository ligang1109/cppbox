//
// Created by ligang on 18-11-27.
//

#include "buffer_writer.h"

#include "misc/misc.h"

namespace cppbox {

namespace log {

BufferWriter::BufferWriter(WriterSptr &writer, size_t size) :
        writer_sptr_(writer),
        buffer_uptr_(misc::MakeUnique<misc::SimpleBuffer>(size)) {
}

BufferWriter::~BufferWriter() {
  FlushUnlocked();
}

int BufferWriter::Flush() {
  std::lock_guard<std::mutex> lock(mutex_);

  return FlushUnlocked();
}

int BufferWriter::FlushUnlocked() {
  if (buffer_uptr_->Used() != 0) {
    writer_sptr_->Write(buffer_uptr_->Base(), buffer_uptr_->Used());
    buffer_uptr_->Reset();
  }

  return writer_sptr_->Flush();
}

size_t BufferWriter::Write(const char *msg, size_t len) {
  std::lock_guard<std::mutex> lock(mutex_);

  if (len <= buffer_uptr_->Remain()) {
    buffer_uptr_->Append(msg, len);

    return len;
  }

  FlushUnlocked();
  if (len <= buffer_uptr_->Remain()) {
    buffer_uptr_->Append(msg, len);

    return len;
  }

  return writer_sptr_->Write(msg, len);
}

size_t BufferWriter::Write(const std::string &msg) {
  return Write(msg.c_str(), msg.size());
}


}


}