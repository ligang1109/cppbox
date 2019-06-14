//
// Created by ligang on 18-12-28.
//

#include "gtest/gtest.h"

#include "cppbox/net/net.h"

class NetTest : public ::testing::Test {
 protected:
  void SetUp() override {
  }

  void TearDown() override {
  }
};

TEST_F(NetTest, Misc) {
  int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (sockfd == -1) {
    std::cout << cppbox::misc::NewErrorUptrByErrno()->String() << std::endl;
    return;
  }

  auto error_uptr = cppbox::net::BindAndListenForTcpIpV4(sockfd, "127.0.0.1", 8860);
  if (error_uptr != nullptr) {
    std::cout << error_uptr->String() << std::endl;
    return;
  }

  cppbox::net::InetAddress raddr;

  int connfd = cppbox::net::Accept(sockfd, raddr);
  if (connfd == -1) {
    std::cout << "accept error: "
              << cppbox::misc::NewErrorUptrByErrno()->String()
              << std::endl;
    return;
  }

  std::cout << "remote ip is " << raddr.ip << std::endl;
  std::cout << "remote port is " << raddr.port << std::endl;

  ::sleep(10);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}