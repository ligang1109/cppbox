//
// Created by ligang on 19-3-21.
//

#include "http_request.h"

#include "cppbox/misc/misc.h"


namespace cppbox {

namespace net {


std::string HttpRequest::method() {
  return method_;
}

void HttpRequest::set_method(const std::string &method) {
  method_ = method;
}

std::string HttpRequest::raw_url() {
  return raw_url_;
}

void HttpRequest::set_raw_url(const std::string &raw_url) {
  raw_url_ = raw_url;
}

std::string HttpRequest::raw_body() {
  return raw_body_;
}

void HttpRequest::set_raw_body(const std::string &raw_body) {
  raw_body_ = raw_body;
}

std::string HttpRequest::raw_path() {
  return raw_path_;
}


std::string HttpRequest::HeaderValue(const std::string &field) {
  auto it = header_map_.find(field);
  if (it == header_map_.end()) {
    return "";
  }

  return it->second;
}

void HttpRequest::AddHeader(const std::string &field, const std::string &value) {
  auto it = header_map_.find(field);
  if (it == header_map_.end()) {
    header_map_.emplace(field, value);
  } else {
    it->second = value;
  }
}

std::string HttpRequest::QueryValue(const std::string &key) {
  auto it = query_values_.find(key);
  if (it == query_values_.end()) {
    return "";
  }

  return it->second;
}

void HttpRequest::ConvertFromData(HttpParseData &&pdata) {
  method_     = std::move(pdata.method);
  raw_url_    = std::move(pdata.raw_url);
  raw_body_   = std::move(pdata.raw_body);
  raw_path_   = std::move(pdata.raw_path);
  header_map_ = std::move(pdata.header_map);

  if (!pdata.raw_query.empty()) {
    ParseQueryValues(pdata.raw_query.c_str(), pdata.raw_query.size());
  }
}

void HttpRequest::ParseFormBody() {
  if (!raw_body_.empty()) {
    ParseQueryValues(raw_body_.c_str(), raw_body_.size());
  }
}

void HttpRequest::Reset() {
  method_.clear();
  raw_url_.clear();
  raw_body_.clear();
  raw_path_.clear();

  header_map_.clear();
  query_values_.clear();
}

size_t HttpRequest::AppendToBuffer(misc::SimpleBuffer *sbuf_ptr) {
  auto n = sbuf_ptr->Append(method_ + " " + raw_url_ + " HTTP/1.1\r\n");

  for (const auto &header : header_map_) {
    n += sbuf_ptr->Append(header.first + ": " + header.second + "\r\n");
  }

  if (!raw_body_.empty()) {
    n += sbuf_ptr->Append("Content-Length: " + std::to_string(raw_body_.size()) + "\r\n");
  }

  n += sbuf_ptr->Append("\r\n");
  if (!raw_body_.empty()) {
    n += sbuf_ptr->Append(raw_body_);
  }

  return n;
}


void HttpRequest::ParseQueryValues(const char *raw_query_ptr, int len) {
  auto query     = misc::UrlDecode(raw_query_ptr, len);
  auto query_ptr = query.c_str();
  auto query_len = query.size();

  bool parse_value = false;
  int  ks          = 0, kl = 0, vs = 0, vl = 0;

  for (int i = 0; i < query_len; ++i) {
    if (query_ptr[i] != '=') {
      if (parse_value) {
        if (query_ptr[i] == '&') {
          vl = i - vs;
          query_values_.emplace(std::string(query_ptr + ks, kl), std::string(query_ptr + vs, vl));
          parse_value = false;
          ks          = i + 1;
        }
      }
    } else {
      parse_value = true;
      kl          = i - ks;
      vs          = i + 1;
    }
  }

  if (kl) {
    query_values_.emplace(std::string(query_ptr + ks, kl), std::string(query_ptr + vs, query_len - vs));
  }
}


}

}