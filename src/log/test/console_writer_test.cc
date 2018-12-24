//
// Created by ligang on 18-11-27.
//

#include "gtest/gtest.h"

#include "log/base.h"
#include "log/console_writer.h"

class ConsoleWriterTest : public ::testing::Test {
 protected:
  ConsoleWriterTest() {
    writer_ = new cppbox::ConsoleWriter();
  }

  ~ConsoleWriterTest() override {
    delete writer_;
  }

  cppbox::WriterInterface *writer_;
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