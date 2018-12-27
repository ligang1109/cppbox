//
// Created by ligang on 18-12-20.
//

#include "misc.h"

namespace cppbox {

namespace misc {


bool FileExist(const char *path) {
  struct stat statbuf;

  int r = stat(path, &statbuf);
  if (r != 0) {
    return false;
  }

  return true;
}

bool DirExist(const char *path) {
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

std::string FormatTime(time_t ts, const char *layout) {
  if (ts == 0) {
    struct timeval now_time;
    gettimeofday(&now_time, nullptr);
    ts = now_time.tv_sec;
  }
  if (layout == nullptr) {
    layout = kGeneralTimeLayout1;
  }

  char      buf[50];
  struct tm *tmp = localtime(&ts);
  strftime(buf, sizeof(buf), layout, tmp);

  return std::string(buf);
}


}


}