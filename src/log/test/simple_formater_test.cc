//
// Created by ligang on 18-11-28.
//

#include <gtest/gtest.h>

#include "log/base.h"
#include "log/simple_formater.h"

class SimpleFormaterTest : public ::testing::Test {
 protected:
  void SetUp() override {
    formater_ = new cppbox::log::SimpleFormater("abcdefg", "127.0.0.1:12345");
  }

  void TearDown() override {
    delete formater_;
  }

  cppbox::log::FormaterInterface *formater_;
};

TEST_F(SimpleFormaterTest, Format) {
  std::string msg("test formater");

  std::cout << formater_->Format(cppbox::log::LogLevel::kDEBUG, msg);
  std::cout << formater_->Format(cppbox::log::LogLevel::kINFO, msg);
  std::cout << formater_->Format(cppbox::log::LogLevel::kNOTICE, msg);
  std::cout << formater_->Format(cppbox::log::LogLevel::kWARNING, msg);
  std::cout << formater_->Format(cppbox::log::LogLevel::kERROR, msg);
  std::cout << formater_->Format(cppbox::log::LogLevel::kCRITICAL, msg);
  std::cout << formater_->Format(cppbox::log::LogLevel::kALERT, msg);
  std::cout << formater_->Format(cppbox::log::LogLevel::kEMERGENCY, msg);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}