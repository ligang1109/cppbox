//
// Created by ligang on 18-11-23.
//

#include "gtest/gtest.h"

#include "log/base.h"
#include "log/file_writer.h"

class FileWriterTest : public ::testing::Test {
 protected:
  void SetUp() override {
    writer_ = new cppbox::log::FileWriter("/tmp/cppbox_file_writer_test.log");
  }

  void TearDown() override {
    delete writer_;
  }

  cppbox::log::WriterInterface *writer_;
};

TEST_F(FileWriterTest, Write) {
  for (int i = 0; i < 100; i++) {
    std::string msg = "hello " + std::to_string(i) + "\n";
    writer_->Write(msg);
  }

  sleep(10);
  writer_->Write("write after sleep\n");

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}