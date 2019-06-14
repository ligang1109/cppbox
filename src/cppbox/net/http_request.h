//
// Created by ligang on 19-3-21.
//

#ifndef CPPBOX_NET_HTTP_REQUEST_H
#define CPPBOX_NET_HTTP_REQUEST_H

#include "http_parse.h"


namespace cppbox {

namespace net {


class HttpRequest : public misc::NonCopyable {
 public:
  std::string method();

  std::string raw_url();

  std::string raw_body();

  std::string raw_path();

  std::string HeaderValue(const std::string &field);

  std::string QueryValue(const std::string &key);

  void ConvertFromData(HttpParseData &&pdata);

  void ParseFormBody();

  void Reset();

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
