//
// Created by ligang on 18-5-11.
//

#include <iostream>

#include "gtest/gtest.h"

#include "misc/misc.h"

class MiscTest : public ::testing::Test {
 protected:
  MiscTest() {

  }

  ~MiscTest() {

  }
};


TEST_F(MiscTest, File) {
  EXPECT_EQ(cppbox::Misc::FileExist("/home/ligang/devspace/personal/cppsimple/CMakeLists.txt"), true);
  EXPECT_EQ(cppbox::Misc::DirExist("/home/ligang/devspace/personal/cppsimple"), true);
}

TEST_F(MiscTest, Time) {
  std::cout << cppbox::Misc::FormatTime() << std::endl;

  struct timeval now_time;
  gettimeofday(&now_time, nullptr);

  std::cout << cppbox::Misc::FormatTime(now_time.tv_sec - 86400, "%Y%m%d %H:%M:%S") << std::endl;
}

class TestMakeUnique {
 public:
  TestMakeUnique(const std::string &name) : name_(name) {
    std::cout << "construct uptr" << std::endl;
  }

  ~TestMakeUnique() {
    std::cout << "destruct uptr" << std::endl;
  }

  std::string name_;
};

TEST_F(MiscTest, MakeUnique) {
  auto uptr = cppbox::Misc::MakeUnique<TestMakeUnique>(std::string("test make unique"));

  std::cout << uptr->name_ << std::endl;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}