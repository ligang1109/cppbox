//
// Created by ligang on 18-11-27.
//

#ifndef CPPBOX_LOG_BUFFER_WRITER_H
#define CPPBOX_LOG_BUFFER_WRITER_H

#include <memory>
#include <mutex>

#include "base.h"

#include "misc/non_copyable.h"
#include "misc/simple_buffer.h"

namespace cppbox {

class BufferWriter : public WriterInterface, public NonCopyable {
 public:
  explicit BufferWriter(WriterSptr &writer, size_t size = 4096);

  size_t Write(const char *msg, size_t len) override;

  size_t Write(const std::string &msg) override;

  int Flush() override;

  ~BufferWriter() override;

 private:
  WriterSptr       writer_sptr_;
  SimpleBufferUptr buffer_uptr_;

  std::mutex mutex_;

  int FlushUnlocked();
};

}


#endif //CPPBOX_LOG_BUFFER_WRITER_H
