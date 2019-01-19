//
// Created by ligang on 18-11-27.
//

#include "gtest/gtest.h"

#include "log/base.h"
#include "log/buffer_writer.h"
#include "log/console_writer.h"
#include "log/file_writer.h"


class BufferWriterTest : public ::testing::Test {
 protected:
  void SetUp() override {
    cppbox::log::WriterSptr cw = std::make_shared<cppbox::log::ConsoleWriter>();
    console_writer_ = new cppbox::log::BufferWriter(cw, 4096);

    cppbox::log::WriterSptr fw = std::make_shared<cppbox::log::FileWriter>("/tmp/cppbox_buffer_writer_test.log");
    file_writer_ = new cppbox::log::BufferWriter(fw, 4096);
  }

  void TearDown() override {
    delete console_writer_;
    delete file_writer_;
  }

  cppbox::log::WriterInterface *console_writer_;
  cppbox::log::WriterInterface *file_writer_;
};

TEST_F(BufferWriterTest, Write) {
  for (int i = 0; i < 1000; i++) {
    std::string msg = "hello " + std::to_string(i) + "\n";
    console_writer_->Write(msg.c_str(), msg.size());
    file_writer_->Write(msg.c_str(), msg.size());
  }

  sleep(10);
  console_writer_->Write("write after sleep\n");
  file_writer_->Write("write after sleep\n");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}