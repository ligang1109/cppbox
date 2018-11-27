//
// Created by ligang on 18-11-28.
//

#include <gtest/gtest.h>

#include "log/base.h"
#include "log/simple_formater.h"

class SimpleFormaterTest : public ::testing::Test {
 protected:
  SimpleFormaterTest() {
    formater_ = new cppbox::SimpleFormater("abcdefg", "127.0.0.1:12345");
  }

  ~SimpleFormaterTest() override {
    delete formater_;
  }

  cppbox::FormaterInterface *formater_;
};

TEST_F(SimpleFormaterTest, Format) {
  std::string msg("test formater");

  std::cout << formater_->Format(cppbox::LogLevel::kDEBUG, msg);
  std::cout << formater_->Format(cppbox::LogLevel::kINFO, msg);
  std::cout << formater_->Format(cppbox::LogLevel::kNOTICE, msg);
  std::cout << formater_->Format(cppbox::LogLevel::kWARNING, msg);
  std::cout << formater_->Format(cppbox::LogLevel::kERROR, msg);
  std::cout << formater_->Format(cppbox::LogLevel::kCRITICAL, msg);
  std::cout << formater_->Format(cppbox::LogLevel::kALERT, msg);
  std::cout << formater_->Format(cppbox::LogLevel::kEMERGENCY, msg);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}