//
// Created by ligang on 18-11-27.
//

#include "gtest/gtest.h"

#include "base.h"
#include "console_writer.h"

class ConsoleWriterTest : public ::testing::Test {
 protected:
  ConsoleWriterTest() {
    writer_ = new cppbox::ConsoleWriter("/tmp/cppbox_Console_writer_test.log");
  }

  ~ConsoleWriterTest() override {
    delete writer_;
  }

  cppbox::ConsoleWriter *writer_;
};

TEST_F(ConsoleWriterTest, Write) {
  for (int i = 0; i < 100; i++) {
    std::string msg = "hello " + std::to_string(i) + "\n";
    writer_->Write(msg.c_str(), msg.size());
  }

  sleep(10);
  writer_->Write("write after sleep\n");

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}