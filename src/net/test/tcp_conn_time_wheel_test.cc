//
// Created by ligang on 19-4-24.
//

#include "gtest/gtest.h"

#include "net/tcp_conn_time_wheel.h"


class TcpConnTimeWheelTest : public ::testing::Test {
 protected:
  TcpConnTimeWheelTest() :
          loop_uptr_(new cppbox::net::EventLoop()),
          wheel_uptr_(new cppbox::net::TcpConnTimeWheel(loop_uptr_.get())) {
    loop_uptr_->Init();
    wheel_uptr_->Init();
  }

  ~TcpConnTimeWheelTest() override {
  }

  cppbox::net::EventLoopUptr loop_uptr_;
  cppbox::net::TcpConnTimeWheelUptr wheel_uptr_;
};

void TcpConnDestructCallback(cppbox::net::TcpConnection &tcp_conn) {
  std::cout << "destruct tcp_conn " << tcp_conn.connfd() << std::endl;
}

TEST_F(TcpConnTimeWheelTest, Roll) {
  for (auto i = 0; i < 10; ++i) {
    cppbox::net::InetAddress address;
    auto tcp_conn_sptr = std::make_shared<cppbox::net::TcpConnection>(i + 10, address, loop_uptr_.get());
    tcp_conn_sptr->set_destruct_callback(TcpConnDestructCallback);
    wheel_uptr_->AddConnection(tcp_conn_sptr, i + 1);
  }

  loop_uptr_->Loop();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
