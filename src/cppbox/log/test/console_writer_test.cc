//
// Created by ligang on 18-11-27.
//

#include "gtest/gtest.h"

#include "cppbox/log/base.h"
#include "cppbox/log/console_writer.h"

class ConsoleWriterTest : public ::testing::Test {
 protected:
  ConsoleWriterTest() {
    writer_ = new cppbox::log::ConsoleWriter();
  }

  ~ConsoleWriterTest() override {
    delete writer_;
  }

  cppbox::log::WriterInterface *writer_;
};

TEST_F(ConsoleWriterTest, Write) {
  for (int i = 0; i < 100; i++) {
    std::string msg = "hello " + std::to_string(i) + "\n";
    writer_->Write(msg);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}