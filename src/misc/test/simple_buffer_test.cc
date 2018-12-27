//
// Created by ligang on 18-12-11.
//

#include "gtest/gtest.h"

#include "misc/simple_buffer.h"

class SimpleBufferTest : public ::testing::Test {
 protected:
  SimpleBufferTest() {
    sbufp_ = new cppbox::misc::SimpleBuffer(100);
  }

  ~SimpleBufferTest() {
    delete sbufp_;
  }

  cppbox::misc::SimpleBuffer *sbufp_;
};

TEST_F(SimpleBufferTest, Buffer) {
  for (int i = 0; i < 100; i++) {
    std::string msg = "hello " + std::to_string(i) + "\n";
    if (msg.length() > sbufp_->Remain()) {
      std::cout << "buf is full, remain " << sbufp_->Remain() << std::endl;
      std::cout << sbufp_->ToString() << std::endl;
      sbufp_->Reset();
    }

    if (msg.length() > sbufp_->Remain()) {
      std::cout << "msg too long" << std::endl;
      std::cout << msg << std::endl;
    } else {
      std::cout << "append to buf" << std::endl;
      sbufp_->Append(msg);
    }
  }

  std::cout << "output buf" << std::endl;
  std::cout << sbufp_->ToString() << std::endl;
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}