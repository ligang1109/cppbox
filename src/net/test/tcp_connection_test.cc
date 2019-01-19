//
// Created by ligang on 18-12-28.
//

#include <netinet/in.h>
#include <arpa/inet.h>

#include "gtest/gtest.h"

#include "net/tcp_connection.h"
#include "net/net.h"

#include "misc/misc.h"

class TcpConnectionTest : public ::testing::Test {
 protected:
  void SetUp() override {
    event_loop_uptr_ = cppbox::misc::MakeUnique<cppbox::net::EventLoop>();
    EXPECT_TRUE(event_loop_uptr_->Init() == nullptr);

    listenfd_ = cppbox::net::NewTcpIpV4NonBlockSocket();
    EXPECT_TRUE(listenfd_ > 0);
    EXPECT_TRUE(cppbox::net::BindAndListenForTcpIpV4(listenfd_, "127.0.0.1", 8860) == nullptr);

    auto event_sptr = std::make_shared<cppbox::net::Event>(listenfd_);
    event_sptr->set_events(cppbox::net::Event::kReadEvents);
    event_sptr->set_read_callback(std::bind(&TcpConnectionTest::ListenCallback, this, std::placeholders::_1));

    event_loop_uptr_->UpdateEvent(event_sptr);
    event_loop_uptr_->Loop();
  }

  void TearDown() override {
  }

  cppbox::net::EventLoopUptr     event_loop_uptr_;
  cppbox::net::TcpConnectionSptr tcp_conn_sptr_;

 private:
  void ListenCallback(cppbox::misc::SimpleTimeSptr happened_st_sptr) {
    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(struct sockaddr_in));
    socklen_t clientLen = sizeof(struct sockaddr);

    int connfd    = ::accept4(listenfd_, (struct sockaddr *) &clientAddr, &clientLen, SOCK_CLOEXEC | SOCK_NONBLOCK);
    EXPECT_TRUE(connfd > 0);

    char     ip[INET_ADDRSTRLEN];
    EXPECT_TRUE(inet_ntop(AF_INET, &clientAddr.sin_addr, ip, INET_ADDRSTRLEN) != nullptr);
    uint16_t port = ntohs(clientAddr.sin_port);

    tcp_conn_sptr_ = std::make_shared<cppbox::net::TcpConnection>(connfd, ip, port, event_loop_uptr_.get());
    tcp_conn_sptr_->set_connected_callback(std::bind(&TcpConnectionTest::ConnectedCallback, this, std::placeholders::_1, std::placeholders::_2));
    tcp_conn_sptr_->set_disconnected_callback(std::bind(&TcpConnectionTest::DisconnectedCallback, this, std::placeholders::_1, std::placeholders::_2));
    tcp_conn_sptr_->ConnectEstablished(happened_st_sptr);
  }

  void ConnectedCallback(cppbox::net::TcpConnectionSptr tcp_conn_sptr, cppbox::misc::SimpleTimeSptr happened_st_sptr) {
    std::cout << "connected" << std::endl;
    std::cout << tcp_conn_sptr_->peer_ip() << ":" << tcp_conn_sptr_->peer_port() << std::endl;
    std::cout << happened_st_sptr->Format() << std::endl;

    event_loop_uptr_->Quit();
  }

  void DisconnectedCallback(cppbox::net::TcpConnectionSptr tcp_conn_sptr, cppbox::misc::SimpleTimeSptr happened_st_sptr) {
    std::cout << "disconnected" << std::endl;
    std::cout << tcp_conn_sptr_->peer_ip() << ":" << tcp_conn_sptr_->peer_port() << std::endl;
    std::cout << happened_st_sptr->Format() << std::endl;
  }

  int listenfd_;
};

TEST_F(TcpConnectionTest, RW) {
  static int write_complete_cnt = 0;

  tcp_conn_sptr_->set_read_callback(
          [=](cppbox::net::TcpConnectionSptr tcp_conn_sptr, cppbox::misc::SimpleTimeSptr happened_st_sptr) {
            char buf[100];
            auto r = tcp_conn_sptr->Receive(buf, sizeof(buf));
            if (r > 0) {
              auto sr = tcp_conn_sptr->Send(buf, r);
              if (sr == r) {
                write_complete_cnt++;
                if (write_complete_cnt == 3) {
                  event_loop_uptr_->Quit();
                }
              }
            }
          });

  tcp_conn_sptr_->set_write_complete_callback(
          [=](cppbox::net::TcpConnectionSptr tcp_conn_sptr, cppbox::misc::SimpleTimeSptr happened_st_sptr) {
            std::cout << "write complete" << std::endl;
            write_complete_cnt++;

            if (write_complete_cnt == 3) {
              event_loop_uptr_->Quit();
              return;
            }
          });

  event_loop_uptr_->Loop();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}