//
// Created by ligang on 18-12-28.
//

#include <netinet/in.h>
#include <arpa/inet.h>

#include <fstream>

#include "gtest/gtest.h"

#include "net/tcp_connection.h"
#include "net/net.h"

#include "misc/misc.h"

class TcpConnectionTest : public ::testing::Test {
 protected:
  TcpConnectionTest() {
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

  ~TcpConnectionTest() override {
    ::close(listenfd_);
  }

  cppbox::net::EventLoopUptr event_loop_uptr_;
  cppbox::net::TcpConnectionSptr tcp_conn_sptr_;

 private:
  void ListenCallback(const cppbox::misc::SimpleTimeSptr &happen_st_sptr) {
    cppbox::net::InetAddress raddr;
    int connfd = Accept(listenfd_, raddr);

    tcp_conn_sptr_ = std::make_shared<cppbox::net::TcpConnection>(connfd, raddr, event_loop_uptr_.get());
    tcp_conn_sptr_->set_connected_callback(std::bind(&TcpConnectionTest::ConnectedCallback, this, std::placeholders::_1, std::placeholders::_2));
    tcp_conn_sptr_->set_disconnected_callback(std::bind(&TcpConnectionTest::DisconnectedCallback, this, std::placeholders::_1, std::placeholders::_2));
    tcp_conn_sptr_->ConnectEstablished(happen_st_sptr);
  }

  void ConnectedCallback(const cppbox::net::TcpConnectionSptr &tcp_conn_sptr, const cppbox::misc::SimpleTimeSptr &happen_st_sptr) {
    std::cout << "connected" << std::endl;
    std::cout << tcp_conn_sptr_->remote_ip() << ":" << tcp_conn_sptr_->remote_port() << std::endl;
    std::cout << happen_st_sptr->Format() << std::endl;

    event_loop_uptr_->Quit();
  }

  void DisconnectedCallback(const cppbox::net::TcpConnectionSptr &tcp_conn_sptr, const cppbox::misc::SimpleTimeSptr &happen_st_sptr) {
    std::cout << "disconnected" << std::endl;
    std::cout << tcp_conn_sptr_->remote_ip() << ":" << tcp_conn_sptr_->remote_port() << std::endl;
    std::cout << happen_st_sptr->Format() << std::endl;
  }

  int listenfd_;
};

TEST_F(TcpConnectionTest, Echo) {
  static int write_complete_cnt = 0;

  tcp_conn_sptr_->set_read_callback(
          [=](cppbox::net::TcpConnectionSptr tcp_conn_sptr, cppbox::misc::SimpleTimeSptr happen_st_sptr) {
            std::cout << "connected time is " << tcp_conn_sptr->connected_time_sptr()->Format() << std::endl;
            std::cout << "last receive time is " << tcp_conn_sptr->last_receive_time_sptr()->Format() << std::endl;

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
          [=](cppbox::net::TcpConnectionSptr tcp_conn_sptr, cppbox::misc::SimpleTimeSptr happen_st_sptr) {
            std::cout << "write complete" << std::endl;
            write_complete_cnt++;

            if (write_complete_cnt == 3) {
              event_loop_uptr_->Quit();
              return;
            }
          });

  event_loop_uptr_->Loop();
}

TEST_F(TcpConnectionTest, File) {
  tcp_conn_sptr_->set_read_callback(
          [=](cppbox::net::TcpConnectionSptr tcp_conn_sptr, cppbox::misc::SimpleTimeSptr happen_st_sptr) {
            char buf[100];
            auto r = tcp_conn_sptr->Receive(buf, sizeof(buf));
            std::string path(buf, r - 2);
            if (!cppbox::misc::FileExist(path.c_str())) {
              std::cout << "file not exist" << std::endl;
              return;
            }

            auto wbuf_uptr = cppbox::misc::MakeUnique<cppbox::misc::SimpleBuffer>();
            auto fp = ::fopen(path.c_str(), "r");
            char rbuf[256];
            bool has_error = false;

            for (auto i = 0; i < 10000; ++i) {
              if (has_error) {
                break;
              }

              ::rewind(fp);
              while (true) {
                auto n = ::fread(rbuf, 1, sizeof(rbuf), fp);
                if (n == 0) {
                  std::cout << "read eof" << std::endl;
                  break;
                } else if (n < 0) {
                  std::cout << "read error: " << cppbox::misc::NewErrorUptrByErrno()->String() << std::endl;
                  has_error = true;
                  break;
                } else {
                  wbuf_uptr->Append(rbuf, n);
                }
              }
            }

            ::fclose(fp);

            std::cout << "readable " << wbuf_uptr->Readable() << std::endl;
            tcp_conn_sptr->Send(wbuf_uptr->ReadBegin(), wbuf_uptr->Readable());
          });

  tcp_conn_sptr_->set_write_complete_callback(
          [=](cppbox::net::TcpConnectionSptr tcp_conn_sptr, cppbox::misc::SimpleTimeSptr happen_st_sptr) {
            std::cout << "write complete" << std::endl;
            event_loop_uptr_->Quit();
          });

  event_loop_uptr_->Loop();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}