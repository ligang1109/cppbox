//
// Created by ligang on 19-1-3.
//

#include "tcp_connection.h"

#include <unistd.h>

namespace cppbox {

namespace net {

TcpConnection::TcpConnection(int connfd, const char *peer_ip, uint16_t peer_port) :
        connfd_(connfd),
        peer_ip_(peer_ip),
        peer_port_(peer_port) {}

TcpConnection::~TcpConnection() {
  close(connfd_);
}

int TcpConnection::connfd() {
  return connfd_;
}

std::string TcpConnection::peer_ip() {
  return peer_ip_;
}

uint16_t TcpConnection::peer_port() {
  return peer_port_;
}


}

}