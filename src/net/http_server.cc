//
// Created by ligang on 19-3-21.
//

#include <iostream>
#include "http_server.h"

namespace cppbox {

namespace net {


HttpConnection::HttpConnection() :
        hstatus_(HttpConnectionStatus::kWaitRequest),
        pdata_uptr_(new HttpParseData()),
        parser_uptr_(new HttpParser(pdata_uptr_.get())),
        request_uptr_(new HttpRequest()),
        response_uptr_(new HttpResponse()) {}

HttpConnectionStatus HttpConnection::hstatus() {
  return hstatus_;
}

void HttpConnection::set_hstatus(HttpConnectionStatus hstatus) {
  hstatus_ = hstatus;
}

HttpRequest *HttpConnection::Request() {
  return request_uptr_.get();
}

HttpResponse *HttpConnection::Response() {
  return response_uptr_.get();
}

bool HttpConnection::ParseRequest(misc::SimpleBuffer *rbuf_ptr) {
  auto len = rbuf_ptr->Readable();
  auto n   = parser_uptr_->Execute(rbuf_ptr->ReadBegin(), len);
  if (n != len) {
    return false;
  }

  rbuf_ptr->AddReadIndex(n);
  if (pdata_uptr_->parse_complete) {
    hstatus_ = HttpConnectionStatus::KParseRequestComplete;
    request_uptr_->ConvertFromData(std::move(*pdata_uptr_));
  }

  return true;
}

void HttpConnection::SendError(const TcpConnectionSptr &tcp_conn_sptr, int code, const std::string &msg) {
  response_uptr_->SetStatus(code, msg);
  hstatus_ = HttpConnectionStatus::kWaitClose;

  SendResponse(tcp_conn_sptr);
}

void HttpConnection::SendResponse(const TcpConnectionSptr &tcp_conn_sptr) {
  auto write_buf_ptr = tcp_conn_sptr->WriteBuffer();

  response_uptr_->AppendToBuffer(write_buf_ptr);

  auto len = write_buf_ptr->Readable();
  auto n   = tcp_conn_sptr->SendWriteBuffer();
  if (n == -1) {
    tcp_conn_sptr->Close(false);
    return;
  }

  if (n == len) {
    RequestProcessComplete(tcp_conn_sptr);
  }
}

void HttpConnection::RequestProcessComplete(const TcpConnectionSptr &tcp_conn_sptr) {
  if (hstatus_ == HttpConnectionStatus::kWaitClose) {
    tcp_conn_sptr->Close();
    return;
  }

  hstatus_ = HttpConnectionStatus::kWaitRequest;

  pdata_uptr_->Reset();
  parser_uptr_->Reset();
  request_uptr_->Reset();
  response_uptr_->Reset();
}


HttpServer::HttpServer(uint16_t port, const std::string &ip, uint16_t default_conn_idle_seconds) :
        server_(port, ip, default_conn_idle_seconds) {}

misc::ErrorUptr HttpServer::Init(int conn_thread_cnt, int conn_thread_loop_timeout_ms, int init_evlist_size) {
  return server_.Init(conn_thread_cnt, conn_thread_loop_timeout_ms, init_evlist_size);
}

misc::ErrorUptr HttpServer::Start() {
  server_.set_connected_callback(
          std::bind(
                  &HttpServer::ConnectedCallback, this,
                  std::placeholders::_1,
                  std::placeholders::_2));

  server_.set_read_callback(
          std::bind(
                  &HttpServer::ReadCallback, this,
                  std::placeholders::_1,
                  std::placeholders::_2));

  server_.set_write_complete_callback(
          std::bind(
                  &HttpServer::WriteCompleteCallback, this,
                  std::placeholders::_1,
                  std::placeholders::_2));

  return server_.Start();
}

void HttpServer::AddHandleFunc(const std::string &path, const HttpHandleFunc &hf) {
  handle_map_.emplace(path, hf);
}

void HttpServer::SetLogger(log::LoggerInterface *logger_ptr) {
  server_.set_logger_ptr(logger_ptr);
}

void HttpServer::ConnectedCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happened_st_sptr) {
  tcp_conn_sptr->set_data_sptr(std::make_shared<HttpConnection>());
}

void HttpServer::ReadCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happened_st_sptr) {
  auto http_conn_sptr = std::static_pointer_cast<HttpConnection>(tcp_conn_sptr->data_sptr());

  bool parse_ok;
  switch (http_conn_sptr->hstatus()) {
    case HttpConnectionStatus::kWaitRequest:
      http_conn_sptr->set_hstatus(HttpConnectionStatus::kParseRequest);
    case HttpConnectionStatus::kParseRequest:
      parse_ok = http_conn_sptr->ParseRequest(tcp_conn_sptr->ReadBuffer());
      break;
    default:
      return;
  }

  if (!parse_ok) {
    http_conn_sptr->SendError(tcp_conn_sptr, 400, "Bad Request");
    return;
  }
  if (http_conn_sptr->hstatus() == HttpConnectionStatus::KParseRequestComplete) {
    ProcessRequest(tcp_conn_sptr, http_conn_sptr);
  }
}

void HttpServer::WriteCompleteCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happened_st_sptr) {
  auto http_conn_sptr = std::static_pointer_cast<HttpConnection>(tcp_conn_sptr->data_sptr());

  http_conn_sptr->RequestProcessComplete(tcp_conn_sptr);
}

void HttpServer::ProcessRequest(const TcpConnectionSptr &tcp_conn_sptr, const HttpConnectionSptr &http_conn_sptr) {
  http_conn_sptr->set_hstatus(HttpConnectionStatus::kProcessRequest);

  auto it = handle_map_.find(http_conn_sptr->Request()->raw_path());
  if (it == handle_map_.end()) {
    http_conn_sptr->SendError(tcp_conn_sptr, 404, "Not Found");
    return;
  }

  it->second(tcp_conn_sptr, http_conn_sptr);
}


}

}