//
// Created by ligang on 18-12-20.
//


#include "simple_buffer.h"

namespace cppbox {

namespace misc {

SimpleBuffer::SimpleBuffer(size_t init_size, size_t max_size) :
        buf_(init_size),
        max_size_(max_size),
        read_index_(0),
        write_index_(0) {}

bool SimpleBuffer::AddReadIndex(size_t len) {
  auto result = read_index_ + len;
  if (result > write_index_) {
    return false;
  }

  read_index_ += len;
  CheckReset();

  return true;
}

bool SimpleBuffer::AddWriteIndex(size_t len) {
  auto result = write_index_ + len;
  if (result > buf_.size()) {
    return false;
  }

  write_index_ += len;

  return true;
}

size_t SimpleBuffer::Size() {
  return buf_.size();
}

size_t SimpleBuffer::Readable() {
  return write_index_ - read_index_;
}

size_t SimpleBuffer::Writeable() {
  return buf_.size() - write_index_;
}

void SimpleBuffer::Reset() {
  read_index_  = 0;
  write_index_ = 0;
}

bool SimpleBuffer::Resize(size_t size) {
  if (max_size_ > 0) {
    if (size > max_size_) {
      return false;
    }
  }

  buf_.resize(size);

  return true;
}

size_t SimpleBuffer::Append(const char *data, size_t len) {
  if (!EnsureWriteSize(len)) {
    return 0;
  }

  std::copy(data, data + len, buf_.begin() + write_index_);
  write_index_ += len;

  return len;
}

size_t SimpleBuffer::Append(const std::string &data) {
  return Append(data.c_str(), data.length());
}

char *SimpleBuffer::ReadBegin() {
  return buf_.begin().base() + read_index_;
}

char *SimpleBuffer::WriteBegin() {
  return buf_.begin().base() + write_index_;
}

size_t SimpleBuffer::Read(char *data, size_t len) {
  auto readable = Readable();
  if (readable == 0) {
    return 0;
  }

  if (len > readable) {
    len = readable;
  }

  std::copy(buf_.begin() + read_index_, buf_.begin() + read_index_ + len, data);
  read_index_ += len;

  CheckReset();

  return len;
}

std::string SimpleBuffer::ReadAsString(size_t len) {
  auto readable = Readable();
  if (readable == 0) {
    return "";
  }

  if (len > readable) {
    len = readable;
  }

  std::string s = std::string(ReadBegin(), len);
  read_index_ += len;

  CheckReset();

  return s;
}


std::string SimpleBuffer::ReadAllAsString() {
  std::string s = std::string(ReadBegin(), Readable());

  Reset();

  return s;
}

void SimpleBuffer::CheckReset() {
  if (read_index_ == write_index_) {
    Reset();
  }
}

bool SimpleBuffer::EnsureWriteSize(size_t write_len) {
  if (write_len <= Writeable()) {
    return true;
  }

  if (write_len <= (read_index_ + Writeable())) {
    auto bi = buf_.begin();
    std::copy(bi + read_index_, bi + write_index_, bi);
    write_index_ = Readable();
    read_index_  = 0;

    return true;
  }

  auto need = write_index_ + write_len;
  auto size = buf_.size() * 2;
  while (size < need) {
    size *= 2;
  }

  if (max_size_ > 0) {
    if (size > max_size_) {
      size = max_size_;
    }
    if (size < need) {
      return false;
    }
  }

  buf_.resize(size);

  return true;
}


}

}