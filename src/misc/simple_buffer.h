//
// Created by ligang on 18-12-11.
//

#ifndef CPPBOX_MISC_SIMPLE_BUFFER_H
#define CPPBOX_MISC_SIMPLE_BUFFER_H

#include <memory>
#include <vector>

#include "non_copyable.h"

namespace cppbox {

namespace misc {


class SimpleBuffer : public NonCopyable {
 public:
  explicit SimpleBuffer(size_t size);

  size_t Remain();

  size_t Append(const char *data, size_t len);

  size_t Append(const std::string &data);

  void Reset();

  const char *Base();

  size_t Used();

  std::string ToString();

 private:
  std::vector<char> buf_;
  size_t            used_;
};

using SimpleBufferUptr = std::unique_ptr<SimpleBuffer>;


}


}


#endif //CPPBOX_MISC_SIMPLE_BUFFER_H
