//
// Created by ligang on 18-11-23.
//

#include "gtest/gtest.h"

#include "base.h"
#include "file_writer.h"

class FileWriterTest : public ::testing::Test {
 protected:
  FileWriterTest() {
    writer_ = new cppbox::FileWriter("/tmp/cppbox_file_writer_test.log");
  }

  ~FileWriterTest() override {
    delete writer_;
  }

  cppbox::FileWriter *writer_;
};

TEST_F(FileWriterTest, Write) {
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