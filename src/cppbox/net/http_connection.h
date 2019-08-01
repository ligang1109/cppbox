//
// Created by ligang on 7/23/19.
//

#ifndef CPPBOX_NET_HTTP_CONNECTION_H
#define CPPBOX_NET_HTTP_CONNECTION_H

#include "http_parse.h"
#include "http_response.h"
#include "http_request.h"
#include "tcp_connection.h"

#include "cppbox/misc/non_copyable.h"


namespace cppbox {

namespace net {


enum class HttpConnectionStatus {
  kWaitData      = 1,
  kParseData     = 2,
  kParseComplete = 3,
  kProcessData   = 4,
  kWaitClose     = 5,
};

class HttpConnection : public TcpConnection {
 public:
  explicit HttpConnection(int connfd, const InetAddress &address, EventLoop *loop_ptr, size_t read_protected_size = 4096);

  HttpConnectionStatus hstatus();

  void set_hstatus(HttpConnectionStatus hstatus);

  HttpRequest *Request();

  HttpResponse *Response();

  bool ParseRequest();

  bool ParseResponse();

  void SendError(int code, const std::string &msg);

  void SendResponse();

  void SendRequest();

  void RequestProcessComplete();

  void ResetMore() override;

 private:
  HttpConnectionStatus hstatus_;

  HttpParseDataUptr pdata_uptr_;
  HttpParserUptr    parser_uptr_;

  HttpRequestUptr  request_uptr_;
  HttpResponseUptr response_uptr_;
};

using HttpConnectionSptr = std::shared_ptr<HttpConnection>;


}

}


#endif //CPPBOX_NET_HTTP_CONNECTION_H
