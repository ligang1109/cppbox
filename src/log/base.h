//
// Created by ligang on 18-11-21.
//

#ifndef CPPBOX_LOG_BASE_H
#define CPPBOX_LOG_BASE_H

#include <string>
#include <vector>

namespace cppbox {

enum LogLevel {
  kEMERGENCY = 0,
  kALERT     = 1,
  kCRITICAL  = 2,
  kERROR     = 3,
  kWARNING   = 4,
  kNOTICE    = 5,
  kINFO      = 6,
  kDEBUG     = 7,
};

static const std::vector<std::string> kLogLevelMsgList({"emergency",
                                                        "alert",
                                                        "critical",
                                                        "error",
                                                        "warning",
                                                        "notice",
                                                        "info",
                                                        "debug"});

class LoggerInterface {
 public:
  virtual ~LoggerInterface() = default;

  virtual void Debug(const std::string &msg) = 0;

  virtual void Debug(const std::initializer_list<std::string> &partList) = 0;

  virtual void Info(const std::string &msg) = 0;

  virtual void Info(const std::initializer_list<std::string> &partList) = 0;

  virtual void Notice(const std::string &msg) = 0;

  virtual void Notice(const std::initializer_list<std::string> &partList) = 0;

  virtual void Warning(const std::string &msg) = 0;

  virtual void Warning(const std::initializer_list<std::string> &partList) = 0;

  virtual void Error(const std::string &msg) = 0;

  virtual void Error(const std::initializer_list<std::string> &partList) = 0;

  virtual void Critical(const std::string &msg) = 0;

  virtual void Critical(const std::initializer_list<std::string> &partList) = 0;

  virtual void Alert(const std::string &msg) = 0;

  virtual void Alert(const std::initializer_list<std::string> &partList) = 0;

  virtual void Emergency(const std::string &msg) = 0;

  virtual void Emergency(const std::initializer_list<std::string> &partList) = 0;

  virtual void Log(LogLevel level, const std::string &msg) = 0;

  virtual void Log(LogLevel level, const std::initializer_list<std::string> &partList) = 0;
};

class WriterInterface {
 public:
  virtual ~WriterInterface() = default;

  virtual size_t Write(const char *msg, size_t len) = 0;

  virtual size_t Write(const std::string &msg) = 0;

  virtual int Flush() = 0;
};

}


#endif //CPPBOX_LOG_BASE_H
