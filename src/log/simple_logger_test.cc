//
// Created by ligang on 18-11-21.
//

#include "gtest/gtest.h"

#include "base.h"
#include "simple_logger.h"

class SimpleLoggerTest : public ::testing::Test {
 protected:
  SimpleLoggerTest() {
    logger_ = new cppbox::SimpleLogger();
  }

  ~SimpleLoggerTest() override {
    delete logger_;
  }

  cppbox::LoggerInterface *logger_;
};

TEST_F(SimpleLoggerTest, Log) {
  logger_->Debug("hello");
  logger_->Debug({"abc", "bcd", "cde"});

  logger_->Info("hello");
  logger_->Info({"abc", "bcd", "cde"});

  logger_->Notice("hello");
  logger_->Notice({"abc", "bcd", "cde"});

  logger_->Warning("hello");
  logger_->Warning({"abc", "bcd", "cde"});

  logger_->Error("hello");
  logger_->Error({"abc", "bcd", "cde"});

  logger_->Critical("hello");
  logger_->Critical({"abc", "bcd", "cde"});

  logger_->Alert("hello");
  logger_->Alert({"abc", "bcd", "cde"});

  logger_->Emergency("hello");
  logger_->Emergency({"abc", "bcd", "cde"});
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
