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


/// +-------------+------------- --+-  -------------+
/// |             |    readable    |    writable    |
/// |             |                |                |
/// +-------------+------------- --+-  -------------+
/// |             |                |                |
/// 0         read_index      write_index         size

class SimpleBuffer : public NonCopyable {
 public:
  explicit SimpleBuffer(size_t init_size = 1024, size_t max_size = 0);

  bool AddReadIndex(size_t len);

  bool AddWriteIndex(size_t len);

  size_t Size();

  size_t Readable();

  size_t Writeable();

  void Reset();

  bool Resize(size_t size);

  size_t Append(const char *data, size_t len);

  size_t Append(const std::string &data);

  char *ReadBegin();

  char *WriteBegin();

  size_t Read(char *data, size_t len);

  std::string ReadAsString(size_t len);

  std::string ReadAllAsString();

 private:
  void CheckReset();

  bool EnsureWriteSize(size_t write_len);

  std::vector<char> buf_;

  size_t max_size_;
  size_t read_index_;
  size_t write_index_;
};

using SimpleBufferUptr = std::unique_ptr<SimpleBuffer>;


}


}


#endif //CPPBOX_MISC_SIMPLE_BUFFER_H
