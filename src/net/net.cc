//
// Created by ligang on 18-12-28.
//

#include "net.h"

#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>


namespace cppbox {

namespace net {


int NewTcpIpV4NonBlockSocket() {
  return ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
}


misc::ErrorUptr SetSockOpt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
  if (::setsockopt(sockfd, level, optname, optval, optlen) == -1) {
    return misc::NewErrorUptrByErrno();
  }

  return nullptr;
}

misc::ErrorUptr SetReuseAddr(int sockfd) {
  int optval = 1;

  return SetSockOpt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
}

misc::ErrorUptr BindForTcpIpV4(int sockfd, const char *ip, uint16_t port) {
  struct sockaddr_in serverAddr;
  memset(&serverAddr, 0, sizeof(struct sockaddr_in));

  serverAddr.sin_family = AF_INET;
  int r = inet_pton(AF_INET, ip, &(serverAddr.sin_addr));
  if (r == 0) {
    return misc::NewErrorUptr(misc::Error::kInvalidArg, "invalid ip");
  }

  serverAddr.sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
    return misc::NewErrorUptrByErrno();
  }

  return nullptr;
}

misc::ErrorUptr Listen(int sockfd, int backlog) {
  if (listen(sockfd, backlog) == -1) {
    return misc::NewErrorUptrByErrno();
  }

  return nullptr;
}


misc::ErrorUptr BindAndListenForTcpIpV4(int sockfd, const char *ip, uint16_t port, bool reuseaddr, int backlog) {
  if (reuseaddr) {
    auto error_uptr = SetReuseAddr(sockfd);
    if (error_uptr != nullptr) {
      return error_uptr;
    }
  }

  auto error_uptr = BindForTcpIpV4(sockfd, ip, port);
  if (error_uptr != nullptr) {
    return error_uptr;
  }

  error_uptr = Listen(sockfd, backlog);
  if (error_uptr != nullptr) {
    return error_uptr;
  }

  return nullptr;
}



}

}