//
// Created by ligang on 19-2-7.
//

#include "gtest/gtest.h"

#include "net/tcp_server.h"

class TcpServerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    server_uptr_.reset(new cppbox::net::TcpServer(8860));
  }

  void TearDown() override {
  }

  cppbox::net::TcpServerUptr server_uptr_;
};

TEST_F(TcpServerTest, Serve) {
//  server_uptr_->Start(10);

  std::cout << "You can use telnet to test the server" << std::endl;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}