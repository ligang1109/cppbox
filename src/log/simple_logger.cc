//
// Created by ligang on 18-11-21.
//

#include "simple_logger.h"

#include <iostream>

namespace cppbox {

SimpleLogger::SimpleLogger() {
  std::cout << "construct" << std::endl;
}

SimpleLogger::~SimpleLogger() {
  std::cout << "destruct" << std::endl;
}

void SimpleLogger::Debug(const std::string &msg) {
  Log(LogLevel::kDEBUG, msg);
}

void SimpleLogger::Debug(const std::initializer_list<std::string> &partList) {
  Log(LogLevel::kDEBUG, partList);
}

void SimpleLogger::Info(const std::string &msg) {
  Log(LogLevel::kINFO, msg);
}

void SimpleLogger::Info(const std::initializer_list<std::string> &partList) {
  Log(LogLevel::kINFO, partList);
}

void SimpleLogger::Notice(const std::string &msg) {
  Log(LogLevel::kNOTICE, msg);
}

void SimpleLogger::Notice(const std::initializer_list<std::string> &partList) {
  Log(LogLevel::kNOTICE, partList);
}

void SimpleLogger::Warning(const std::string &msg) {
  Log(LogLevel::kWARNING, msg);
}

void SimpleLogger::Warning(const std::initializer_list<std::string> &partList) {
  Log(LogLevel::kWARNING, partList);
}

void SimpleLogger::Error(const std::string &msg) {
  Log(LogLevel::kERROR, msg);
}

void SimpleLogger::Error(const std::initializer_list<std::string> &partList) {
  Log(LogLevel::kERROR, partList);
}

void SimpleLogger::Critical(const std::string &msg) {
  Log(LogLevel::kCRITICAL, msg);
}

void SimpleLogger::Critical(const std::initializer_list<std::string> &partList) {
  Log(LogLevel::kCRITICAL, partList);
}

void SimpleLogger::Alert(const std::string &msg) {
  Log(LogLevel::kALERT, msg);
}

void SimpleLogger::Alert(const std::initializer_list<std::string> &partList) {
  Log(LogLevel::kALERT, partList);
}

void SimpleLogger::Emergency(const std::string &msg) {
  Log(LogLevel::kEMERGENCY, msg);
}

void SimpleLogger::Emergency(const std::initializer_list<std::string> &partList) {
  Log(LogLevel::kEMERGENCY, partList);

}

void SimpleLogger::Log(LogLevel level, const std::string &msg) {
  std::cout << kLogLevelMsgList[level]
            << " "
            << msg
            << " "
            << std::endl;
}

void SimpleLogger::Log(LogLevel level, const std::initializer_list<std::string> &partList) {
  if (partList.size() == 0) {
    return;
  }

  auto it = partList.begin();

  std::string msg(*it);
  ++it;

  while (it != partList.end()) {
    msg += " " + *it;
    ++it;
  }

  Log(level, msg);
}

}
