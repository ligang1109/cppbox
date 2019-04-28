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

#include "log/base.h"


namespace cppbox {

namespace net {

enum class HttpConnectionStatus {
  kWaitRequest          = 1,
  kParseRequest         = 2,
  KParseRequestComplete = 3,
  kProcessRequest       = 4,
  kWaitClose            = 5,
};

class HttpConnection : misc::NonCopyable {
 public:
  HttpConnection();

  HttpConnectionStatus hstatus();

  void set_hstatus(HttpConnectionStatus hstatus);

  HttpRequest *Request();

  HttpResponse *Response();

  bool ParseRequest(misc::SimpleBuffer *rbuf_ptr);

  void SendError(const TcpConnectionSptr &tcp_conn_sptr, int code, const std::string &msg);

  void SendResponse(const TcpConnectionSptr &tcp_conn_sptr);

  void RequestProcessComplete(const TcpConnectionSptr &tcp_conn_sptr);

 private:
  HttpConnectionStatus hstatus_;

  HttpParseDataUptr pdata_uptr_;
  HttpParserUptr    parser_uptr_;

  HttpRequestUptr  request_uptr_;
  HttpResponseUptr response_uptr_;
};

using HttpConnectionSptr = std::shared_ptr<HttpConnection>;

using HttpHandleFunc = std::function<void(const TcpConnectionSptr &, const HttpConnectionSptr &)>;

class HttpServer : public misc::NonCopyable {
 public:

  explicit HttpServer(uint16_t port, const std::string &ip = "127.0.0.1", uint16_t default_conn_idle_seconds = 300);

  misc::ErrorUptr Init(int conn_thread_cnt, int conn_thread_loop_timeout_ms, int init_evlist_size = 1024);

  misc::ErrorUptr Start();

  void AddHandleFunc(const std::string &path, const HttpHandleFunc &hf);

  void SetLogger(log::LoggerInterface *logger_ptr);

 private:
  void ConnectedCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happened_st_sptr);

  void ReadCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happened_st_sptr);

  void WriteCompleteCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happened_st_sptr);

  void ProcessRequest(const TcpConnectionSptr &tcp_conn_sptr, const HttpConnectionSptr &http_conn_sptr);

  TcpServer server_;

  std::map<std::string, HttpHandleFunc> handle_map_;
};


}

}

#endif //CPPBOX_NET_HTTP_SERVER_H
