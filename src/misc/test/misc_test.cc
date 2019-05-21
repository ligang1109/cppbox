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
  }

  void TearDown() override {
  }
};


TEST_F(MiscTest, File) {
  EXPECT_EQ(cppbox::misc::FileExist("/home/ligang/devspace/personal/cppsimple/CMakeLists.txt"), true);
  EXPECT_EQ(cppbox::misc::DirExist("/home/ligang/devspace/personal/cppsimple"), true);
}

class TestMakeUnique {
 public:
  explicit TestMakeUnique(const char *name) : name_(name) {
    std::cout << "construct uptr" << std::endl;
  }

  ~TestMakeUnique() {
    std::cout << "destruct uptr" << std::endl;
  }

  std::string name_;
};

TEST_F(MiscTest, MakeUnique) {
  auto uptr = cppbox::misc::MakeUnique<TestMakeUnique>("test make unique");

  std::cout << uptr->name_ << std::endl;
}

TEST_F(MiscTest, UrlEncodeDecode) {
  std::string query  = "a=1&b=2&c=3";
  std::string equery = cppbox::misc::UrlEncode(query.c_str(), query.size());

  std::cout << equery << std::endl;
  std::cout << cppbox::misc::UrlDecode(equery.c_str(), equery.size()) << std::endl;

  equery = "ab%3D1%26bcd%3D2%26cdef%3D3%26defgh%3Dasdfasdfsa";
  std::cout << cppbox::misc::UrlDecode(equery.c_str(), equery.size()) << std::endl;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}