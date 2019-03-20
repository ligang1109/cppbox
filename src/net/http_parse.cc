//
// Created by ligang on 19-3-20.
//

#include <http-parser/http_parser.h>
#include "http_parse.h"

#include "curl/curl.h"

namespace cppbox {

namespace net {


int ParseOnUrl(http_parser *parser, const char *at, size_t len) {
  auto pdata_ptr = static_cast<HttpParseData *>(parser->data);

  pdata_ptr->raw_url.append(at, len);

  return 0;
}

int ParseOnHeaderField(http_parser *parser, const char *at, size_t len) {
  auto pdata_ptr = static_cast<HttpParseData *>(parser->data);

  if (pdata_ptr->last_parsed_was_value) {
    if (!pdata_ptr->last_header_field.empty()) {
      pdata_ptr->header_map[pdata_ptr->last_header_field] = pdata_ptr->last_header_value;
    }

    pdata_ptr->last_header_field.clear();
    pdata_ptr->last_header_value.clear();
  }

  pdata_ptr->last_header_field.append(at, len);
  pdata_ptr->last_parsed_was_value = false;

  return 0;
}

int ParseOnHeaderValue(http_parser *parser, const char *at, size_t len) {
  auto pdata_ptr = static_cast<HttpParseData *>(parser->data);

  pdata_ptr->last_header_value.append(at, len);
  pdata_ptr->last_parsed_was_value = true;

  return 0;
}

int ParseOnMessageBegin(http_parser *parser) {
  return 0;
}

int ParseOnHeadersComplete(http_parser *parser) {
  auto pdata_ptr = static_cast<HttpParseData *>(parser->data);

  pdata_ptr->header_map[pdata_ptr->last_header_field] = pdata_ptr->last_header_value;
  pdata_ptr->method = http_method_str(static_cast<http_method>(parser->method));

  const char *raw_url_ptr = pdata_ptr->raw_url.c_str();
  struct http_parser_url ur;
  http_parser_url_init(&ur);
  if (http_parser_parse_url(raw_url_ptr, pdata_ptr->raw_url.size(), 0, &ur) == 0) {
    pdata_ptr->raw_path.append(raw_url_ptr, ur.field_data[UF_PATH].len);

    if ((ur.field_set & (1 << UF_QUERY)) != 0) {
      pdata_ptr->raw_query.append(raw_url_ptr + ur.field_data[UF_QUERY].off, ur.field_data[UF_QUERY].len);
    }
  }

  return 0;
}

int ParseOnBody(http_parser *parser, const char *at, size_t len) {
  auto pdata_ptr = static_cast<HttpParseData *>(parser->data);

  pdata_ptr->raw_body.append(at, len);

  return 0;
}

int ParseOnMessageComplete(http_parser *parser) {
  auto pdata_ptr = static_cast<HttpParseData *>(parser->data);

  pdata_ptr->parse_complete = true;

  return 0;
}


HttpParser::HttpParser(HttpParseData *pdata) {
  parser_.data = pdata;
  http_parser_init(&parser_, HTTP_REQUEST);

  settings_.on_url = ParseOnUrl;
  settings_.on_header_field = ParseOnHeaderField;
  settings_.on_header_value = ParseOnHeaderValue;
  settings_.on_headers_complete = ParseOnHeadersComplete;
  settings_.on_body = ParseOnBody;
  settings_.on_message_begin = ParseOnMessageBegin;
  settings_.on_message_complete = ParseOnMessageComplete;
}


size_t HttpParser::Execute(char *buf, size_t len) {
  return http_parser_execute(&parser_, &settings_, buf, len);
}


}

}