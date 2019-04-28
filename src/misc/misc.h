//
// Created by ligang on 18-11-27.
//

#ifndef CPPBOX_MISC_H
#define CPPBOX_MISC_H

#include <string>
#include <memory>

#include "error.h"

namespace cppbox {

namespace misc {

bool FileExist(const char *path);

bool DirExist(const char *path);

template<typename T, typename ... Ts>
std::unique_ptr<T> MakeUnique(Ts &&...params) {
  return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

ErrorUptr SetFdNonBlock(int fd);

}

}

#endif //CPPBOX_MISC_H
