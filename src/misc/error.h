//
// Created by ligang on 18-12-29.
//

#ifndef CPPBOX_MISC_ERROR_H
#define CPPBOX_MISC_ERROR_H

#include <errno.h>
#include <string.h>

#include <memory>

namespace cppbox {

namespace misc {

class Error {
 public:
  static const int kInvalidArg = 11;

  explicit Error(int errval, const char *errmsg);

  ~Error() = default;

  int errval();

  std::string errmsg();

  std::string String();

 private:
  int         errval_;
  std::string errmsg_;
};

using ErrorUptr = std::unique_ptr<Error>;
using ErrorSptr = std::shared_ptr<Error>;

ErrorUptr NewErrorUptr(int errval, const char *errmsg);

ErrorSptr NewErrorSptr(int errval, const char *errmsg);

ErrorUptr NewErrorUptrByErrno();

ErrorSptr NewErrorSptrByErrno();


}

}

#endif //CPPBOX_MISC_ERROR_H
