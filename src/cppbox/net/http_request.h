//
// Created by ligang on 19-3-21.
//

#ifndef CPPBOX_NET_HTTP_REQUEST_H
#define CPPBOX_NET_HTTP_REQUEST_H

#include "cppbox/misc/simple_buffer.h"

#include "http_parse.h"


namespace cppbox {

namespace net {


class HttpRequest : public misc::NonCopyable {
 public:
  std::string method();

  void set_method(const std::string &method);

  std::string raw_url();

  void set_raw_url(const std::string &raw_url);

  std::string raw_body();

  void set_raw_body(const std::string &raw_body);

  std::string raw_path();

  std::string HeaderValue(const std::string &field);

  void AddHeader(const std::string &field, const std::string &value);

  std::string QueryValue(const std::string &key);

  void ConvertFromData(HttpParseData &&pdata);

  void ParseFormBody();

  void Reset();

  size_t AppendToBuffer(misc::SimpleBuffer *sbuf_ptr);

 private:
  void ParseQueryValues(const char *query_ptr, int len);

  std::string method_;
  std::string raw_url_;
  std::string raw_body_;
  std::string raw_path_;

  std::map<std::string, std::string> header_map_;
  std::map<std::string, std::string> query_values_;
};

using HttpRequestUptr = std::unique_ptr<HttpRequest>;


}

}


#endif //CPPBOX_NET_HTTP_REQUEST_H
