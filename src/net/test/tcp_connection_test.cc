//
// Created by ligang on 18-12-28.
//

#include "gtest/gtest.h"

#include "net/tcp_connection.h"

class TcpConnectionTest : public ::testing::Test {
 protected:
  TcpConnectionTest() {
  }

  ~TcpConnectionTest() override {
  }
};

TEST_F(TcpConnectionTest, Misc) {

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}