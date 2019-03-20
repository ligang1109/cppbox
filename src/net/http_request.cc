//
// Created by ligang on 19-3-21.
//

#include "http_request.h"

#include "curl/curl.h"

namespace cppbox {

namespace net {


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
  method_ = std::move(pdata.method);
  raw_url_ = std::move(pdata.raw_url);
  raw_body_ = std::move(pdata.raw_body);
  raw_path_ = std::move(pdata.raw_path);

  header_map_ = std::move(pdata.header_map);
  if (!pdata.raw_query.empty()) {
    parseQueryValues(pdata.raw_query.c_str(), pdata.raw_query.size());
  }
}

void HttpRequest::ParseFormBody() {
  parseQueryValues(raw_body_.c_str(), raw_body_.size());
}

void HttpRequest::parseQueryValues(const char *raw_query_ptr, int len) {
  CURL *curl = curl_easy_init();
  int outlength;
  auto query_ptr = curl_easy_unescape(curl, raw_query_ptr, len, &outlength);

  bool parse_value = false;
  int ks = 0, kl = 0, vs = 0, vl = 0;

  for (int i = 0; i < outlength; ++i) {
    if (query_ptr[i] != '=') {
      if (parse_value) {
        if (query_ptr[i] == '&') {
          vl = i - vs;
          query_values_[std::string(query_ptr + ks, kl)] = std::string(query_ptr + vs, vl);
          parse_value = false;
          ks = i + 1;
        }
      }
    } else {
      parse_value = true;
      kl = i - ks;
      vs = i + 1;
    }
  }
  if (kl) {
    query_values_[std::string(query_ptr + ks, kl)] = std::string(query_ptr + vs, outlength - vs);
  }

  curl_free(query_ptr);
  curl_easy_cleanup(curl);
}

}

}