//
// Created by ligang on 19-3-20.
//

#ifndef CPPBOX_NET_HTTP_PARSE_H
#define CPPBOX_NET_HTTP_PARSE_H

#include <string>
#include <map>
#include <memory>

#include "cppbox/misc/non_copyable.h"

#include "http-parser/http_parser.h"

namespace cppbox {

namespace net {


struct HttpParseData {
  bool        last_parsed_was_value;
  std::string last_header_field;
  std::string last_header_value;

  bool parse_complete;

  std::string method;
  std::string raw_url;
  std::string raw_body;
  std::string raw_path;
  std::string raw_query;

  int         status_code;
  std::string status_msg;

  std::map<std::string, std::string> header_map;

  void Reset();
};

using HttpParseDataUptr = std::unique_ptr<HttpParseData>;

int ParseOnMessageBegin(http_parser *parser);

int ParseOnUrl(http_parser *parser, const char *at, size_t len);

int ParseOnStatus(http_parser *parser, const char *at, size_t len);

int ParseOnHeaderField(http_parser *parser, const char *at, size_t len);

int ParseOnHeaderValue(http_parser *parser, const char *at, size_t len);

int ParseOnHeadersComplete(http_parser *parser);

int ParseOnBody(http_parser *parser, const char *at, size_t len);

int ParseOnMessageComplete(http_parser *parser);

int ParseOnChunkHeader(http_parser *parser);

int ParseOnChunkComplete(http_parser *parser);


class HttpParser : public misc::NonCopyable {
 public:
  explicit HttpParser(HttpParseData *pdata);

  size_t Execute(char *buf, size_t len);

  void Reset();

 private:
  http_parser          parser_;
  http_parser_settings settings_;
};

using HttpParserUptr = std::unique_ptr<HttpParser>;

}

}

#endif //CPPBOX_NET_HTTP_PARSE_H
