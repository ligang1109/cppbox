//
// Created by ligang on 18-12-29.
//

#include "error.h"


namespace cppbox {

namespace misc {


Error::Error(int errval, const char *errmsg) :
        errval_(errval),
        errmsg_(errmsg) {}

int Error::errval() {
  return errval_;
}

std::string Error::errmsg() {
  return errmsg_;
}

std::string Error::String() {
  return "errno: " + std::to_string(errval_) + ", errmsg: " + errmsg_;
}

ErrorUptr NewErrorUptr(int errval, const char *errmsg) {
  return std::unique_ptr<Error>(new Error(errval, errmsg));
}

ErrorSptr NewErrorSptr(int errval, const char *errmsg) {
  return std::make_shared<Error>(errval, errmsg);
}


ErrorUptr NewErrorUptrByErrno() {
  char buf[100];

  return std::unique_ptr<Error>(new Error(errno, strerror_r(errno, buf, sizeof buf)));
}

ErrorSptr NewErrorSptrByErrno() {
  char buf[100];

  return std::make_shared<Error>(errno, strerror_r(errno, buf, sizeof buf));
}


}

}
