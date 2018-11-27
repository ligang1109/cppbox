//
// Created by ligang on 18-11-27.
//

#include "gtest/gtest.h"

#include "base.h"

class BufferTest : public ::testing::Test {
 protected:
  BufferTest() {
    writer_ = new cppbox::Buffer("/tmp/cppbox_file_writer_test.log");
  }

  ~BufferTest() override {
    delete writer_;
  }

  cppbox::Buffer *writer_;
};

TEST_F(BufferTest, Buffer) {
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