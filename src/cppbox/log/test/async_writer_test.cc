//
// Created by ligang on 18-11-27.
//

#include "gtest/gtest.h"

#include "cppbox/log/base.h"
#include "cppbox/log/file_writer.h"
#include "cppbox/log/async_writer.h"


class AsyncWriterTest : public ::testing::Test {
 protected:
  AsyncWriterTest() {
    cppbox::log::WriterSptr fw = std::make_shared<cppbox::log::FileWriter>("/tmp/cppbox_async_writer_test.log", 4096);
    async_writer_ = new cppbox::log::AsyncWriter(fw);
  }

  ~AsyncWriterTest() override {
    delete async_writer_;
  }

  cppbox::log::WriterInterface *async_writer_;
};

TEST_F(AsyncWriterTest, Write) {
  for (int i = 0; i < 1000; ++i) {
    std::string msg = "hello " + std::to_string(i) + "\n";
    async_writer_->Write(msg.c_str(), msg.size());
  }

  sleep(10);
  async_writer_->Write("write after sleep\n");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}