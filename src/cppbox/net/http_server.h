//
// Created by ligang on 19-3-21.
//

#ifndef CPPBOX_NET_HTTP_SERVER_H
#define CPPBOX_NET_HTTP_SERVER_H

#include "tcp_server.h"
#include "http_connection.h"

#include "cppbox/log/base.h"


namespace cppbox {

namespace net {


using HttpHandleFunc = std::function<void(const HttpConnectionSptr &)>;

class HttpServer : public misc::NonCopyable {
 public:
  explicit HttpServer(uint16_t port, const std::string &ip = "127.0.0.1", uint16_t default_conn_idle_seconds = 300);

  misc::ErrorUptr Init(int conn_thread_cnt, int conn_thread_loop_timeout_ms, int init_evlist_size = 1024);

  misc::ErrorUptr Start();

  void AddHandleFunc(const std::string &path, const HttpHandleFunc &hf);

  void SetLogger(log::LoggerInterface *logger_ptr);

 private:
  static HttpConnectionSptr NewConnection(int connfd, const InetAddress &remote_addr, EventLoop *loop_ptr);

  void ReadCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happen_st_sptr);

  static void WriteCompleteCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happen_st_sptr);

  void ProcessRequest(const HttpConnectionSptr &http_conn_sptr);

  TcpServer server_;

  std::map<std::string, HttpHandleFunc> handle_map_;
};


}

}

#endif //CPPBOX_NET_HTTP_SERVER_H
