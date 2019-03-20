//
// Created by ligang on 19-3-20.
//

#include "http_response.h"


namespace cppbox {

namespace net {

HttpResponse::HttpResponse() :
        status_code_(200),
        status_msg_("OK") {}

void HttpResponse::set_body(const std::string &body) {
  body_ = body;
}

void HttpResponse::SetStatus(int code, const std::string &msg) {
  status_code_ = code;
  status_msg_ = msg;
}

void HttpResponse::AddHeader(const std::string &field, const std::string &value) {
  header_map_[field] = value;
}

size_t HttpResponse::AppendToBuffer(misc::SimpleBuffer *sbuf_ptr) {
  auto n = sbuf_ptr->Append("HTTP/1.1 " + std::to_string(status_code_) + " " + status_msg_ + "\r\n");
  if (!body_.empty()) {
    n += sbuf_ptr->Append("Content-Length: " + std::to_string(body_.size()) + "\r\n");
  }

  for (const auto &header : header_map_) {
    n += sbuf_ptr->Append(header.first + ": " + header.second + "\r\n");
  }

  n += sbuf_ptr->Append("\r\n");
  if (!body_.empty()) {
    n += sbuf_ptr->Append(body_);
  }

  return n;
}

}

}