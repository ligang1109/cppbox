//
// Created by ligang on 18-12-29.
//

#include <sys/socket.h>

#include "gtest/gtest.h"

#include "misc/error.h"

class ErrorTest : public ::testing::Test {
 protected:
  ErrorTest() {
  }

  ~ErrorTest() {
  }
};

TEST_F(ErrorTest, Simple) {
  ::socket(1111, SOCK_STREAM, 0);

  auto euptr = cppbox::misc::NewErrorUptrByErrno();

  std::cout << "errno: " << euptr->errval() << std::endl;
  std::cout << "errmsg: " << euptr->errmsg() << std::endl;

  auto esptr = cppbox::misc::NewErrorSptrByErrno();

  std::cout << "errno: " << esptr->errval() << std::endl;
  std::cout << "errmsg: " << esptr->errmsg() << std::endl;

  std::cout << esptr->String() << std::endl;
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}