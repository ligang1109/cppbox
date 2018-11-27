//
// Created by ligang on 18-11-21.
//

#include "gtest/gtest.h"

#include "log/base.h"
#include "log/simple_logger.h"
#include "log/console_writer.h"
#include "log/buffer_writer.h"
#include "log/file_writer.h"
#include "log/simple_formater.h"

class SimpleLoggerTest : public ::testing::Test {
 protected:
  SimpleLoggerTest() {
    std::string log_id("abcdefg");
    std::string address("127.0.0.1:12345");

    cppbox::FormaterSptr fr = std::make_shared<cppbox::SimpleFormater>(log_id, address);
    cppbox::WriterSptr   cw = std::make_shared<cppbox::ConsoleWriter>();
    cppbox::WriterSptr   fw = std::make_shared<cppbox::FileWriter>("/tmp/cppbox_simple_logger_test.log");
    cppbox::WriterSptr   bw = std::make_shared<cppbox::BufferWriter>(fw);

    clogger_ = new cppbox::SimpleLogger(cw, fr, cppbox::LogLevel::kDEBUG);
    flogger_ = new cppbox::SimpleLogger(bw, fr, cppbox::LogLevel::kDEBUG);
  }

  ~SimpleLoggerTest() override {
    delete clogger_;
    delete flogger_;
  }

  cppbox::LoggerInterface *clogger_;
  cppbox::LoggerInterface *flogger_;
};

void log(cppbox::LoggerInterface *logger) {
  logger->Debug("hello");
  logger->Debug({"abc", "bcd", "cde"});

  logger->Info("hello");
  logger->Info({"abc", "bcd", "cde"});

  logger->Notice("hello");
  logger->Notice({"abc", "bcd", "cde"});

  logger->Warning("hello");
  logger->Warning({"abc", "bcd", "cde"});

  logger->Error("hello");
  logger->Error({"abc", "bcd", "cde"});

  logger->Critical("hello");
  logger->Critical({"abc", "bcd", "cde"});

  logger->Alert("hello");
  logger->Alert({"abc", "bcd", "cde"});

  logger->Emergency("hello");
  logger->Emergency({"abc", "bcd", "cde"});

  sleep(10);
  logger->Log(cppbox::LogLevel::kNOTICE, "log after sleep");
}

TEST_F(SimpleLoggerTest, Log) {
  log(clogger_);
  log(flogger_);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
