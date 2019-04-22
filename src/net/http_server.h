//
// Created by ligang on 19-3-21.
//

#ifndef CPPBOX_NET_HTTP_SERVER_H
#define CPPBOX_NET_HTTP_SERVER_H

#include "tcp_server.h"
#include "http_parse.h"
#include "http_response.h"
#include "http_request.h"

#include "misc/non_copyable.h"


namespace cppbox {

namespace net {

enum class HttpConnectionStatus {
  kWaitRequest = 1,
  kParseRequest = 2,
  KParseRequestComplete = 3,
  kProcessRequest = 4,
  kWaitClose = 5,
};

class HttpConnection : public TcpConnection {
 public:
  explicit HttpConnection(int connfd, const InetAddress &address, EventLoop *loop_ptr, size_t read_protected_size = 4096);

  HttpConnectionStatus hstatus();

  void set_hstatus(HttpConnectionStatus hstatus);

  HttpRequest *Request();

  HttpResponse *Response();

  void ParseRequest();

  void RequestProcessComplete();

  void SendError(int code, const std::string &msg);

  void SendResponse();

 private:
  HttpConnectionStatus hstatus_;

  HttpParseDataUptr pdata_uptr_;
  HttpParserUptr parser_uptr_;

  HttpRequestUptr request_uptr_;
  HttpResponseUptr response_uptr_;
};

using HttpConnectionSptr = std::shared_ptr<HttpConnection>;

using HttpHandleFunc = std::function<void(const HttpConnectionSptr &)>;

class HttpServer : public misc::NonCopyable {
 public:

  explicit HttpServer(uint16_t port, const std::string &ip = "127.0.0.1", uint16_t default_conn_idle_seconds = 300);

  misc::ErrorUptr Init(int conn_thread_cnt, int conn_thread_loop_timeout_ms, int init_evlist_size = 1024);

  misc::ErrorUptr Start();

  void AddHandleFunc(const std::string &path, const HttpHandleFunc &hf);

 private:
  HttpConnectionSptr NewConnection(int connfd, const InetAddress &remote_addr, EventLoop *loop_ptr);

  void ReadCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happened_st_sptr);

  void WriteCompleteCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happened_st_sptr);

  void ProcessRequest(const HttpConnectionSptr &http_conn_sptr);

  TcpServer server_;

  std::map<std::string, HttpHandleFunc> handle_list_;
};


}

}

#endif //CPPBOX_NET_HTTP_SERVER_H
