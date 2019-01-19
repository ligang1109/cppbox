//
// Created by ligang on 18-5-11.
//

#include <sys/time.h>

#include <iostream>


#include "gtest/gtest.h"

#include "misc/misc.h"

class MiscTest : public ::testing::Test {
 protected:
  void SetUp() override {
    std::cout << "misc test setup" << std::endl;
  }

  void TearDown() override {
    std::cout << "misc test teardown" << std::endl;
  }
};


TEST_F(MiscTest, File) {
  EXPECT_EQ(cppbox::misc::FileExist("/home/ligang/devspace/personal/cppsimple/CMakeLists.txt"), true);
  EXPECT_EQ(cppbox::misc::DirExist("/home/ligang/devspace/personal/cppsimple"), true);
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
  auto uptr = cppbox::misc::MakeUnique<TestMakeUnique>(std::string("test make unique"));

  std::cout << uptr->name_ << std::endl;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}