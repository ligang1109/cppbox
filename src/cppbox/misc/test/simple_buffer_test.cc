//
// Created by ligang on 18-12-11.
//

#include "gtest/gtest.h"

#include "cppbox/misc/simple_buffer.h"

class SimpleBufferTest : public ::testing::Test {
 protected:
  SimpleBufferTest() {
    sbufp_ = new cppbox::misc::SimpleBuffer(100);
  }

  ~SimpleBufferTest() override {
    delete sbufp_;
  }

  cppbox::misc::SimpleBuffer *sbufp_;
};

TEST_F(SimpleBufferTest, Buffer) {
  for (int i = 0; i < 100; i++) {
    std::string msg = "hello " + std::to_string(i) + "\n";
    if (msg.length() > sbufp_->Writeable()) {
      std::cout << "buf is full, writeable " << sbufp_->Writeable() << std::endl;
      std::cout << sbufp_->ReadAllAsString() << std::endl;
    }

    if (msg.length() > sbufp_->Writeable()) {
      std::cout << "msg too long" << std::endl;
      std::cout << msg << std::endl;
    } else {
      std::cout << "append to buf" << std::endl;
      sbufp_->Append(msg);
    }
  }

  std::cout << "output buf" << std::endl;
  std::cout << sbufp_->ReadAllAsString() << std::endl;

  std::string s("hello");
  char buf[100];

  sbufp_->Append(s);
  auto n = sbufp_->Read(buf, s.size());

  EXPECT_EQ(n, s.size());
  for (auto i = 0; i < n; ++i) {
    std::cout << buf[i];
  }
  std::cout << std::endl;

  sbufp_->Append(s);
  auto s1 = sbufp_->ReadAsString(s.size());

  EXPECT_EQ(s, s1);
  std::cout << s1 << std::endl;

  sbufp_->Append(s);
  auto s2 = sbufp_->ReadAllAsString();

  EXPECT_EQ(s, s2);
  std::cout << s2 << std::endl;

  for (int i = 0; i < 100; i++) {
    sbufp_->Append("hello " + std::to_string(i) + "\n");
  }

  std::cout << sbufp_->ReadAllAsString() << std::endl;
  std::cout << sbufp_->Size() << std::endl;
}

//TEST_F(SimpleBufferTest, Max) {
//  cppbox::misc::SimpleBuffer sbuf(10, 100);
//  while (true) {
//    auto r = sbuf.Append("a", 1);
//    std::cout << sbuf.Size() << std::endl;
//    if (r == 0) {
//      break;
//    }
//  }
//
//  std::cout << sbuf.Readable() << std::endl;
//}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}