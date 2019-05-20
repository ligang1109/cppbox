//
// Created by ligang on 19-3-21.
//

#include "http_request.h"


namespace cppbox {

namespace net {

HttpRequest::HttpRequest() :
        curl_(nullptr) {}

HttpRequest::~HttpRequest() {
  if (curl_ != nullptr) {
    curl_easy_cleanup(curl_);
  }
}


std::string HttpRequest::method() {
  return method_;
}

std::string HttpRequest::raw_url() {
  return raw_url_;
}

std::string HttpRequest::raw_body() {
  return raw_body_;
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

void HttpRequest::ParseQueryValues(const char *raw_query_ptr, int len) {
  int  outlength;
  auto query_ptr = curl_easy_unescape(curl(), raw_query_ptr, len, &outlength);

  bool parse_value = false;
  int  ks          = 0, kl = 0, vs = 0, vl = 0;

  for (int i = 0; i < outlength; ++i) {
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
    query_values_.emplace(std::string(query_ptr + ks, kl), std::string(query_ptr + vs, outlength - vs));
  }

  curl_free(query_ptr);
}

CURL *HttpRequest::curl() {
  if (curl_ == nullptr) {
    curl_ = curl_easy_init();
  }

  return curl_;
}


}

}