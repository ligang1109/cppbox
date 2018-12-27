//
// Created by ligang on 18-12-20.
//


#include "simple_buffer.h"

namespace cppbox {

namespace misc {

SimpleBuffer::SimpleBuffer(size_t size) : buf_(size), used_(0) {}

size_t SimpleBuffer::Remain() {
  return buf_.size() - used_;
}

size_t SimpleBuffer::Append(const char *data, size_t len) {
  if (len > Remain()) {
    return 0;
  }

  std::copy(data, data + len, buf_.begin() + used_);
  used_ += len;

  return len;
}

size_t SimpleBuffer::Append(const std::string &data) {
  return Append(data.c_str(), data.length());
}

void SimpleBuffer::Reset() {
  used_ = 0;
}

const char *SimpleBuffer::Base() {
  return buf_.begin().base();
}

size_t SimpleBuffer::Used() {
  return used_;
}

std::string SimpleBuffer::ToString() {
  return std::string(Base(), Used());
}


}


}