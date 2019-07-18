//
// Created by ligang on 18-12-28.
//

#ifndef CPPBOX_NET_H
#define CPPBOX_NET_H

#include <sys/types.h>
#include <sys/socket.h>

#include "cppbox/misc/error.h"


namespace cppbox {

namespace net {

static const int kDefaultBacklog = 128;


int NewTcpIpV4NonBlockSocket();

misc::ErrorUptr SetSockOpt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);

misc::ErrorUptr SetReuseAddr(int sockfd);

misc::ErrorUptr BindForTcpIpV4(int sockfd, const char *ip, uint16_t port);

misc::ErrorUptr Listen(int sockfd, int backlog = kDefaultBacklog);

misc::ErrorUptr BindAndListenForTcpIpV4(int sockfd, const char *ip, uint16_t port, bool reuseaddr = true, int backlog = kDefaultBacklog);


struct InetAddress {
  std::string ip;
  uint16_t    port;
};

int Accept(int listenfd, struct InetAddress &address, int flags = SOCK_CLOEXEC | SOCK_NONBLOCK);

misc::ErrorUptr Connect(int sockfd, const char *ip, uint16_t port);


}

}


#endif //CPPBOX_NET_H
