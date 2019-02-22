//
// Created by ligang on 18-12-20.
//

#include "misc.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


namespace cppbox {

namespace misc {


bool FileExist(const char *path) {
  struct stat statbuf;

  int r = ::stat(path, &statbuf);
  if (r != 0) {
    return false;
  }

  return true;
}

bool DirExist(const char *path) {
  struct stat statbuf;

  int r = ::stat(path, &statbuf);
  if (r != 0) {
    return false;
  }

  if (S_ISDIR(statbuf.st_mode)) {
    return true;
  }

  return false;
}


ErrorUptr SetFdNonBlock(int fd) {
  int flags;
  if ((flags = ::fcntl(fd, F_GETFL, NULL)) == -1) {
    return NewErrorUptrByErrno();
  }

  if (!(flags & O_NONBLOCK)) {
    if (::fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
      return NewErrorUptrByErrno();
    }
  }

  return nullptr;
}


}

}