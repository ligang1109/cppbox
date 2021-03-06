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

char IntToHex(char code) {
  static char hex[] = "0123456789abcdef";

  return hex[code & 15];
}

char HexToInt(char ch) {
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

std::string UrlEncode(const char *str, int len) {
  std::string result;

  for (int i = 0; i < len; ++i) {
    if (isalnum(str[i]) || str[i] == '-' || str[i] == '_' || str[i] == '.' || str[i] == '~') {
      result.append(1, str[i]);
    } else if (str[i] == ' ') {
      result.append(1, '+');
    } else {
      result.append(1, '%');
      result.append(1, IntToHex(str[i] >> 4));
      result.append(1, IntToHex(str[i] & 15));
    }
  }

  return result;
}

std::string UrlDecode(const char *str, int len) {
  std::string result;

  for (int i = 0; i < len; ++i) {
    if (str[i] == '%') {
      if ((i + 2) >= len) {
        return "";
      }
      result.append(1, (HexToInt(str[i + 1]) << 4) | HexToInt(str[i + 2]));
      i += 2;
    } else if (str[i] == '+') {
      result.append(1, ' ');
    } else {
      result.append(1, str[i]);
    }
  }

  return result;
}


}

}