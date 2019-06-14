//
// Created by ligang on 18-11-28.
//

#include <gtest/gtest.h>

#include "cppbox/log/base.h"
#include "cppbox/log/simple_formater.h"

class SimpleFormaterTest : public ::testing::Test {
 protected:
  SimpleFormaterTest() {
  }

  ~SimpleFormaterTest() override {
  }

};

TEST_F(SimpleFormaterTest, Format) {
  std::string msg("test formater");
  
  cppbox::log::SimpleFormater f1;
  std::cout << f1.Format(cppbox::log::LogLevel::kDEBUG, msg);
  std::cout << f1.Format(cppbox::log::LogLevel::kINFO, msg);
  std::cout << f1.Format(cppbox::log::LogLevel::kNOTICE, msg);
  std::cout << f1.Format(cppbox::log::LogLevel::kWARNING, msg);
  std::cout << f1.Format(cppbox::log::LogLevel::kERROR, msg);
  std::cout << f1.Format(cppbox::log::LogLevel::kCRITICAL, msg);
  std::cout << f1.Format(cppbox::log::LogLevel::kALERT, msg);
  std::cout << f1.Format(cppbox::log::LogLevel::kEMERGENCY, msg);

  cppbox::log::SimpleFormater f2("%Y%m%d%H%M%S");
  std::cout << f2.Format(cppbox::log::LogLevel::kDEBUG, msg);
  std::cout << f2.Format(cppbox::log::LogLevel::kINFO, msg);
  std::cout << f2.Format(cppbox::log::LogLevel::kNOTICE, msg);
  std::cout << f2.Format(cppbox::log::LogLevel::kWARNING, msg);
  std::cout << f2.Format(cppbox::log::LogLevel::kERROR, msg);
  std::cout << f2.Format(cppbox::log::LogLevel::kCRITICAL, msg);
  std::cout << f2.Format(cppbox::log::LogLevel::kALERT, msg);
  std::cout << f2.Format(cppbox::log::LogLevel::kEMERGENCY, msg);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}