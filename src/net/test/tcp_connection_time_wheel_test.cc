//
// Created by ligang on 19-4-24.
//

#include "gtest/gtest.h"

#include "net/tcp_connection_time_wheel.h"

void Timeoutallback(const cppbox::net::TcpConnectionSptr &tcp_conn_sptr, const cppbox::misc::SimpleTimeSptr &happen_st_sptr) {
  std::cout << "timeout tcp_conn " << tcp_conn_sptr->connfd() << std::endl;
}

class TcpConnectionTimeWheelTest : public ::testing::Test {
 protected:
  TcpConnectionTimeWheelTest() :
          loop_uptr_(new cppbox::net::EventLoop()),
          wheel_uptr_(new cppbox::net::TcpConnectionTimeWheel(loop_uptr_.get(), Timeoutallback)) {
    loop_uptr_->Init();
    wheel_uptr_->Init();
  }

  ~TcpConnectionTimeWheelTest() override {
  }

  cppbox::net::EventLoopUptr              loop_uptr_;
  cppbox::net::TcpConnectionTimeWheelUptr wheel_uptr_;
};


TEST_F(TcpConnectionTimeWheelTest, Roll) {
  for (auto i = 0; i < 10; ++i) {
    cppbox::net::InetAddress address;

    auto tcp_conn_sptr = std::make_shared<cppbox::net::TcpConnection>(i + 10, address, loop_uptr_.get());
    wheel_uptr_->AddConnection(tcp_conn_sptr, i + 1);
  }

  loop_uptr_->Loop();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
