//
// Created by ligang on 19-1-3.
//

#ifndef CPPBOX_NET_TCP_CONNECTION_H
#define CPPBOX_NET_TCP_CONNECTION_H

#include <string>
#include <memory>

#include "misc/non_copyable.h"

namespace cppbox {

namespace net {


class TcpConnection : public misc::NonCopyable {
 public:
  explicit TcpConnection(int connfd, const char *peer_ip, uint16_t peer_port);

  ~TcpConnection();

  int connfd();

  std::string peer_ip();

  uint16_t peer_port();

  int Read();

  int Write();

 private:
  int         connfd_;
  std::string peer_ip_;
  uint16_t    peer_port_;
};

using TcpConnectionSptr = std::shared_ptr<TcpConnection>;

}

}

#endif //CPPBOX_NET_TCP_CONNECTION_H
