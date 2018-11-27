//
// Created by ligang on 18-11-21.
//

#ifndef CPPBOX_LOG_SIMPLE_LOGGER_H
#define CPPBOX_LOG_SIMPLE_LOGGER_H

#include "base.h"

namespace cppbox {

class SimpleLogger : public LoggerInterface {
 public:
  SimpleLogger();

  ~SimpleLogger() override;

  void Debug(const std::string &msg) override;

  void Debug(const std::initializer_list<std::string> &partList) override;

  void Info(const std::string &msg) override;

  void Info(const std::initializer_list<std::string> &partList) override;

  void Notice(const std::string &msg) override;

  void Notice(const std::initializer_list<std::string> &partList) override;

  void Warning(const std::string &msg) override;

  void Warning(const std::initializer_list<std::string> &partList) override;

  void Error(const std::string &msg) override;

  void Error(const std::initializer_list<std::string> &partList) override;

  void Critical(const std::string &msg) override;

  void Critical(const std::initializer_list<std::string> &partList) override;

  void Alert(const std::string &msg) override;

  void Alert(const std::initializer_list<std::string> &partList) override;

  void Emergency(const std::string &msg) override;

  void Emergency(const std::initializer_list<std::string> &partList) override;

  void Log(LogLevel level, const std::string &msg) override;

  void Log(LogLevel level, const std::initializer_list<std::string> &partList) override;
};

}


#endif //CPPBOX_LOG_SIMPLE_LOGGER_H