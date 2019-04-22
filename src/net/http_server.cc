//
// Created by ligang on 19-3-21.
//

#include "http_server.h"

namespace cppbox {

namespace net {


HttpConnection::HttpConnection(int connfd, const InetAddress &address, EventLoop *loop_ptr, size_t read_protected_size) :
        TcpConnection(connfd, address, loop_ptr, read_protected_size),
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

void HttpConnection::ParseRequest() {
  auto len = read_buf_uptr_->Readable();
  auto n = parser_uptr_->Execute(read_buf_uptr_->ReadBegin(), len);
  if (n != len) {
    SendError(400, "Bad Request");
    return;
  }

  read_buf_uptr_->AddReadIndex(n);
  if (!pdata_uptr_->parse_complete) {
    return;
  }

  request_uptr_->ConvertFromData(std::move(*pdata_uptr_));
  hstatus_ = HttpConnectionStatus::KParseRequestComplete;
}

void HttpConnection::RequestProcessComplete() {
  if (hstatus_ == HttpConnectionStatus::kWaitClose) {
    Close();
    return;
  }

  hstatus_ = HttpConnectionStatus::kWaitRequest;

  pdata_uptr_.reset(new HttpParseData);
  parser_uptr_.reset(new HttpParser(pdata_uptr_.get()));
  request_uptr_.reset(new HttpRequest);
  response_uptr_.reset(new HttpResponse);
}

void HttpConnection::SendError(int code, const std::string &msg) {
  response_uptr_->SetStatus(code, msg);
  hstatus_ = HttpConnectionStatus::kWaitClose;

  SendResponse();
}


void HttpConnection::SendResponse() {
  auto write_buf_ptr = WriteBuffer();
  response_uptr_->AppendToBuffer(write_buf_ptr);
  auto len = write_buf_ptr->Readable();

  if (SendWriteBuffer() == len) {
    RequestProcessComplete();
  }
}


HttpServer::HttpServer(uint16_t port, const std::string &ip, uint16_t default_conn_idle_seconds) :
        server_(port, ip, default_conn_idle_seconds) {}

misc::ErrorUptr HttpServer::Init(int conn_thread_cnt, int conn_thread_loop_timeout_ms, int init_evlist_size) {
  return server_.Init(conn_thread_cnt, conn_thread_loop_timeout_ms, init_evlist_size);
}

misc::ErrorUptr HttpServer::Start() {
  server_.set_new_conn_func(
          std::bind(
                  &HttpServer::NewConnection, this,
                  std::placeholders::_1,
                  std::placeholders::_2,
                  std::placeholders::_3));

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
  handle_list_[path] = hf;
}

HttpConnectionSptr HttpServer::NewConnection(int connfd, const InetAddress &remote_addr, EventLoop *loop_ptr) {
  return std::make_shared<HttpConnection>(connfd, remote_addr, loop_ptr);
}

void HttpServer::ReadCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happened_st_sptr) {
  auto http_conn_sptr = std::static_pointer_cast<HttpConnection>(tcp_conn_sptr);

  switch (http_conn_sptr->hstatus()) {
    case HttpConnectionStatus::kWaitRequest:
      http_conn_sptr->set_hstatus(HttpConnectionStatus::kParseRequest);
    case HttpConnectionStatus::kParseRequest:
      http_conn_sptr->ParseRequest();
      break;
    default:
      return;
  }

  if (http_conn_sptr->hstatus() == HttpConnectionStatus::KParseRequestComplete) {
    ProcessRequest(http_conn_sptr);
  }
}

void HttpServer::WriteCompleteCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happened_st_sptr) {
  auto http_conn_sptr = std::static_pointer_cast<HttpConnection>(tcp_conn_sptr);

  http_conn_sptr->RequestProcessComplete();
}

void HttpServer::ProcessRequest(const HttpConnectionSptr &http_conn_sptr) {
  http_conn_sptr->set_hstatus(HttpConnectionStatus::kProcessRequest);

  auto it = handle_list_.find(http_conn_sptr->Request()->raw_path());
  if (it == handle_list_.end()) {
    http_conn_sptr->SendError(404, "Not Found");
    return;
  }

  it->second(http_conn_sptr);
}


}

}