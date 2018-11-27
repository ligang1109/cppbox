//
// Created by ligang on 18-5-11.
//

#include <iostream>

#include "gtest/gtest.h"

#include "misc.h"

class MiscTest : public ::testing::Test {
 protected:
  MiscTest() {

  }

  ~MiscTest() {

  }
};

TEST_F(MiscTest, File) {
  EXPECT_EQ(cppbox::Misc::FileExist("/home/ligang/devspace/hogwarts/cppsimple/CMakeLists.txt"), true);
  EXPECT_EQ(cppbox::Misc::DirExist("/home/ligang/devspace/hogwarts/cppsimple"), true);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}