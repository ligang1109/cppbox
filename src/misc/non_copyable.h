//
// Created by ligang on 18-12-14.
//

#ifndef CPPBOX_MISC_COPYABLE_H
#define CPPBOX_MISC_COPYABLE_H

namespace cppbox {

class NonCopyable {
 public:
  NonCopyable(const NonCopyable &) = delete;

  void operator=(const NonCopyable &) = delete;

 protected:
  NonCopyable() = default;

  ~NonCopyable() = default;
};

}

#endif //CPPBOX_MISC_COPYABLE_H
