//
// Created by ligang on 18-12-24.
//

#ifndef CPPBOX_LOG_ASYNC_WRITER_H
#define CPPBOX_LOG_ASYNC_WRITER_H

#include <mutex>
#include <condition_variable>
#include <thread>

#include "base.h"

#include "misc/non_copyable.h"
#include "misc/simple_buffer.h"

namespace cppbox {

namespace log {


class AsyncWriter : public WriterInterface, public misc::NonCopyable {
 public:
  explicit AsyncWriter(WriterSptr &writer, size_t flush_size = 4096 * 1000, int flush_seconds = 3);

  ~AsyncWriter() override;

  size_t Write(const char *msg, size_t len) override;

  size_t Write(const std::string &msg) override;

  int Flush() override;

 private:
  void WriteThreadFunc(int flush_seconds);

  WriterSptr writer_sptr_;
  size_t     flush_size_;

  misc::SimpleBufferUptr              cur_buffer_uptr_;
  misc::SimpleBufferUptr              next_buffer_uptr_;
  std::vector<misc::SimpleBufferUptr> buffer_list_;

  std::mutex              mutex_;
  std::condition_variable cond_;

  volatile bool running_;
  std::thread   write_thread_;
};


}

}


#endif //CPPBOX_LOG_ASYNC_WRITER_H
