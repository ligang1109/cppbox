//
// Created by ligang on 19-3-21.
//

#include "http_server.h"

namespace cppbox {

namespace net {


HttpServer::HttpServer(uint16_t port, const std::string &ip, uint16_t default_conn_idle_seconds) :
        server_(port, ip, default_conn_idle_seconds) {}

misc::ErrorUptr HttpServer::Init(int conn_thread_cnt, int conn_thread_loop_timeout_ms, int init_evlist_size) {
  return server_.Init(conn_thread_cnt, conn_thread_loop_timeout_ms, init_evlist_size);
}

misc::ErrorUptr HttpServer::Start() {
  server_.set_new_conn_func(
          std::bind(
                  &HttpServer::NewConnection,
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
                  &HttpServer::WriteCompleteCallback,
                  std::placeholders::_1,
                  std::placeholders::_2));

  return server_.Start();
}

void HttpServer::AddHandleFunc(const std::string &path, const HttpHandleFunc &hf) {
  auto it = handle_map_.find(path);
  if (it == handle_map_.end()) {
    handle_map_.emplace(path, hf);
  } else {
    it->second = hf;
  }
}

void HttpServer::SetLogger(log::LoggerInterface *logger_ptr) {
  server_.set_logger_ptr(logger_ptr);
}

HttpConnectionSptr HttpServer::NewConnection(int connfd, const InetAddress &remote_addr, EventLoop *loop_ptr) {
  return std::make_shared<HttpConnection>(connfd, remote_addr, loop_ptr);
}

void HttpServer::ReadCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happen_st_sptr) {
  auto http_conn_sptr = std::static_pointer_cast<HttpConnection>(tcp_conn_sptr);

  bool parse_ok;
  switch (http_conn_sptr->hstatus()) {
    case HttpConnectionStatus::kWaitRequest:
      http_conn_sptr->set_hstatus(HttpConnectionStatus::kParseRequest);
    case HttpConnectionStatus::kParseRequest:
      parse_ok = http_conn_sptr->ParseRequest();
      break;
    default:
      return;
  }

  if (!parse_ok) {
    http_conn_sptr->SendError(400, "Bad Request");
    return;
  }
  if (http_conn_sptr->hstatus() == HttpConnectionStatus::KParseRequestComplete) {
    ProcessRequest(http_conn_sptr);
  }
}

void HttpServer::WriteCompleteCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happen_st_sptr) {
  auto http_conn_sptr = std::static_pointer_cast<HttpConnection>(tcp_conn_sptr);

  http_conn_sptr->RequestProcessComplete();
}

void HttpServer::ProcessRequest(const HttpConnectionSptr &http_conn_sptr) {
  http_conn_sptr->set_hstatus(HttpConnectionStatus::kProcessRequest);

  auto it = handle_map_.find(http_conn_sptr->Request()->raw_path());
  if (it == handle_map_.end()) {
    http_conn_sptr->SendError(404, "Not Found");
    return;
  }

  it->second(http_conn_sptr);
}


}

}