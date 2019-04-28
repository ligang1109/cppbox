//
// Created by ligang on 18-12-24.
//

#include "async_writer.h"

#include <functional>

#include "misc/misc.h"


namespace cppbox {

namespace log {


AsyncWriter::AsyncWriter(const WriterSptr &writer, size_t flush_size, int flush_seconds) :
        writer_sptr_(writer),
        flush_size_(flush_size),
        cur_buffer_uptr_(misc::MakeUnique<misc::SimpleBuffer>(flush_size)),
        next_buffer_uptr_(misc::MakeUnique<misc::SimpleBuffer>(flush_size)),
        running_(true),
        write_thread_(std::bind(&AsyncWriter::WriteThreadFunc, this, flush_seconds)) {}

AsyncWriter::~AsyncWriter() {
  running_ = false;
  write_thread_.join();
}

size_t AsyncWriter::Write(const std::string &msg) {
  return Write(msg.c_str(), msg.size());
}

size_t AsyncWriter::Flush() {
  return writer_sptr_->Flush();
}

size_t AsyncWriter::Write(const char *msg, size_t len) {
  std::lock_guard<std::mutex> lock(mutex_);

  if (len <= cur_buffer_uptr_->Writeable()) {
    cur_buffer_uptr_->Append(msg, len);

    return len;
  }

  buffer_list_.push_back(std::move(cur_buffer_uptr_));
  if (next_buffer_uptr_) {
    cur_buffer_uptr_ = std::move(next_buffer_uptr_);
  } else {
    cur_buffer_uptr_.reset(new misc::SimpleBuffer(flush_size_));
  }

  cur_buffer_uptr_->Append(msg, len);
  cond_.notify_one();

  return len;
}


void AsyncWriter::WriteThreadFunc(int flush_seconds) {
  misc::SimpleBufferUptr              cur_buffer_prepare  = misc::MakeUnique<misc::SimpleBuffer>(flush_size_);
  misc::SimpleBufferUptr              next_buffer_prepare = misc::MakeUnique<misc::SimpleBuffer>(flush_size_);
  std::vector<misc::SimpleBufferUptr> buffer_list_to_write;

  while (running_) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if (buffer_list_.empty()) {
        cond_.wait_for(lock, std::chrono::seconds(flush_seconds));
      }

      buffer_list_.push_back(std::move(cur_buffer_uptr_));
      cur_buffer_uptr_ = std::move(cur_buffer_prepare);
      buffer_list_to_write.swap(buffer_list_);
      if (!next_buffer_uptr_) {
        next_buffer_uptr_ = std::move(next_buffer_prepare);
      }
    }

    for (const auto &buffer_uptr : buffer_list_to_write) {
      writer_sptr_->Write(buffer_uptr->ReadBegin(), buffer_uptr->Readable());
    }

    cur_buffer_prepare = std::move(buffer_list_to_write.back());
    buffer_list_to_write.pop_back();
    cur_buffer_prepare->Reset();

    if (!next_buffer_prepare) {
      if (!buffer_list_to_write.empty()) {
        next_buffer_prepare = std::move(buffer_list_to_write.back());
        buffer_list_to_write.pop_back();
        next_buffer_prepare->Reset();
      } else {
        next_buffer_prepare.reset(new misc::SimpleBuffer(flush_size_));
      }
    }

    buffer_list_to_write.clear();
    writer_sptr_->Flush();
  }
}


}

}