//
// Created by ligang on 7/23/19.
//

#include "http_connection.h"

namespace cppbox {

namespace net {


HttpConnection::HttpConnection(int connfd, const InetAddress &address, EventLoop *loop_ptr, size_t read_protected_size) :
        TcpConnection(connfd, address, loop_ptr),
        hstatus_(HttpConnectionStatus::kWaitData),
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

bool HttpConnection::ParseRequest() {
  auto rbuf_ptr = ReadBuffer();
  auto len      = rbuf_ptr->Readable();
  auto n        = parser_uptr_->Execute(rbuf_ptr->ReadBegin(), len);
  if (n != len) {
    return false;
  }

  rbuf_ptr->AddReadIndex(n);
  if (pdata_uptr_->parse_complete) {
    hstatus_ = HttpConnectionStatus::kParseComplete;
    request_uptr_->ConvertFromData(std::move(*pdata_uptr_));
  }

  return true;
}

bool HttpConnection::ParseResponse() {
  auto rbuf_ptr = ReadBuffer();
  auto len      = rbuf_ptr->Readable();
  auto n        = parser_uptr_->Execute(rbuf_ptr->ReadBegin(), len);
  if (n != len) {
    return false;
  }

  rbuf_ptr->AddReadIndex(n);
  if (pdata_uptr_->parse_complete) {
    hstatus_ = HttpConnectionStatus::kParseComplete;
    response_uptr_->ConvertFromData(std::move(*pdata_uptr_));
  }

  return true;
}

void HttpConnection::SendError(int code, const std::string &msg) {
  response_uptr_->SetStatus(code, msg);
  hstatus_ = HttpConnectionStatus::kWaitClose;

  SendResponse();
}

void HttpConnection::SendResponse() {
  auto wbuf_ptr = WriteBuffer();

  response_uptr_->AppendToBuffer(wbuf_ptr);

  auto len = wbuf_ptr->Readable();
  auto n   = SendWriteBuffer();
  if (n == -1) {
    ForceClose();
    return;
  }

  if (n == len) {
    RequestProcessComplete();
  }
}

void HttpConnection::SendRequest() {
  auto wbuf_ptr = WriteBuffer();

  request_uptr_->AppendToBuffer(wbuf_ptr);

  auto len = wbuf_ptr->Readable();
  auto n   = SendWriteBuffer();
  if (n == -1) {
    ForceClose();
    return;
  }

  hstatus_ = HttpConnectionStatus::kWaitData;
}

void HttpConnection::RequestProcessComplete() {
  if (hstatus_ == HttpConnectionStatus::kWaitClose) {
    GracefulClose();
    return;
  }

  ResetMore();
}

void HttpConnection::ResetMore() {
  hstatus_ = HttpConnectionStatus::kWaitData;

  pdata_uptr_->Reset();
  parser_uptr_->Reset();
  request_uptr_->Reset();
  response_uptr_->Reset();
}


}

}