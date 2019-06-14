//
// Created by ligang on 5/24/19.
//

#include "gtest/gtest.h"

#include "cppbox/net/tcp_connection_pool.h"
#include "cppbox/net/tcp_connection.h"


class TcpConnectionPoolTest : public ::testing::Test {
 protected:
  TcpConnectionPoolTest() :
          pool_uptr_(new cppbox::net::TcpConnectionPool(10, 3)) {}

  ~TcpConnectionPoolTest() override {
  }

  cppbox::net::TcpConnectionPoolUptr pool_uptr_;
};


TEST_F(TcpConnectionPoolTest, Roll) {
  cppbox::net::InetAddress address;

  for (auto i = 0; i < 30; ++i) {
    auto success = pool_uptr_->Put(std::make_shared<cppbox::net::TcpConnection>(0, address, nullptr));
    EXPECT_EQ(success, true);
  }
  EXPECT_EQ(pool_uptr_->Full(), true);

  for (auto i = 0; i < 30; ++i) {
    auto tcp_conn_sptr = pool_uptr_->Get();
    EXPECT_TRUE(tcp_conn_sptr != nullptr);
  }
  EXPECT_EQ(pool_uptr_->Empty(), true);

  for (auto i = 0; i < 30; ++i) {
    auto success = pool_uptr_->Put(std::make_shared<cppbox::net::TcpConnection>(0, address, nullptr));
    EXPECT_EQ(success, true);
  }
  EXPECT_EQ(pool_uptr_->Full(), true);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}