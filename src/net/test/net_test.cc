//
// Created by ligang on 18-12-28.
//

#include "gtest/gtest.h"

#include "net/net.h"

class NetTest : public ::testing::Test {
 protected:
  void SetUp() override {
  }

  void TearDown() override {
  }
};

TEST_F(NetTest, Misc) {
  int sockfd = cppbox::net::NewTcpIpV4NonBlockSocket();
  if (sockfd == -1) {
    std::cout << cppbox::misc::NewErrorUptrByErrno()->String() << std::endl;
    return;
  }

  auto error_uptr = cppbox::net::BindAndListenForTcpIpV4(sockfd, "127.0.0.1", 8860);
  if (error_uptr != nullptr) {
    std::cout << error_uptr->String() << std::endl;
    return;
  }

  std::cout << "You can check tcp status use netstat and so on" << std::endl;
  sleep(100);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}