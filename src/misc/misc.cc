//
// Created by ligang on 18-11-27.
//

#include "misc.h"

#include <sys/stat.h>

namespace cppbox {

bool Misc::FileExist(const char *path) {
  struct stat statbuf;

  int r = stat(path, &statbuf);
  if (r != 0) {
    return false;
  }

  return true;
}

bool Misc::DirExist(const char *path) {
  struct stat statbuf;

  int r = stat(path, &statbuf);
  if (r != 0) {
    return false;
  }

  if (S_ISDIR(statbuf.st_mode)) {
    return true;
  }

  return false;
}

}