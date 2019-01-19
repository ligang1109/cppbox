//
// Created by ligang on 19-1-8.
//

#include "gtest/gtest.h"

#include "misc/simple_time.h"

class SimpleTimeTest : public ::testing::Test {
 protected:
  void SetUp() override {
  }

  void TearDown() override {
  }
};

TEST_F(SimpleTimeTest, Time) {
  auto stu = cppbox::misc::NowTimeUptr();

  std::cout << "test now:" << std::endl;
  std::cout << stu->Format() << std::endl;
  std::cout << stu->Format("%Y%m%d %H:%M:%S") << std::endl;
  std::cout << "sec: " << stu->Sec() << std::endl;
  std::cout << "usec: " << stu->Usec() << std::endl;

  sleep(1);
  stu->Update();

  std::cout << "test update:" << std::endl;
  std::cout << stu->Format() << std::endl;
  std::cout << stu->Format("%Y%m%d %H:%M:%S") << std::endl;
  std::cout << "sec: " << stu->Sec() << std::endl;
  std::cout << "usec: " << stu->Usec() << std::endl;

  std::cout << "test parse:" << std::endl;
  stu->ParseTime("2021-10-31 17:16:18", cppbox::misc::kGeneralTimeLayout1);
  std::cout << "sec: " << stu->Sec() << std::endl;
  std::cout << "usec: " << stu->Usec() << std::endl;
  std::cout << stu->Format() << std::endl;

  std::cout << "test add:" << std::endl;
  stu->Add(86400);
  std::cout << "sec: " << stu->Sec() << std::endl;
  std::cout << "usec: " << stu->Usec() << std::endl;
  std::cout << stu->Format() << std::endl;

  std::cout << "test sub:" << std::endl;
  stu->Sub(86400 * 2);
  std::cout << "sec: " << stu->Sec() << std::endl;
  std::cout << "usec: " << stu->Usec() << std::endl;
  std::cout << stu->Format() << std::endl;
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}