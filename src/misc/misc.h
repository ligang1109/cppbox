//
// Created by ligang on 18-11-27.
//

#ifndef CPPBOX_MISC_H
#define CPPBOX_MISC_H

#include <sys/stat.h>
#include <sys/time.h>

#include <string>
#include <memory>

#include "non_copyable.h"

namespace cppbox {

static const char *kGeneralTimeLayout = "%Y-%m-%d %H:%M:%S";

class Misc : public NonCopyable {
 public:
  Misc() = delete;

  static bool FileExist(const char *path);

  static bool DirExist(const char *path);

  static std::string FormatTime(time_t ts = 0, const char *layout = nullptr);

  template<typename T, typename ... Ts>
  static std::unique_ptr<T> MakeUnique(Ts &&...params) {
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
  }
};

}

#endif //CPPBOX_MISC_H
