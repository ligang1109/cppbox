//
// Created by ligang on 19-2-7.
//

#include "gtest/gtest.h"

#include "net/tcp_server.h"


class TcpServerTest : public ::testing::Test {
 protected:
  TcpServerTest() {
  }

  ~TcpServerTest() override {
  }
};


class MyTcpConnection {
 public:
  explicit MyTcpConnection(const std::string &msg) :
          msg_(msg) {}

  ~MyTcpConnection() {
    std::cout << "destruct my_tcp_conn" << std::endl;
  }

  std::string msg() {
    return msg_;
  }

 private:
  std::string msg_;
};


class EchoServer {
 public:
  explicit EchoServer(uint16_t port) :
          server_uptr_(new cppbox::net::TcpServer(8860, "127.0.0.1", 10)) {}

  void Start() {
    server_uptr_->Init(10, -1);

    server_uptr_->set_connected_callback(
            std::bind(
                    &EchoServer::ConnectedCallback, this,
                    std::placeholders::_1,
                    std::placeholders::_2));

    server_uptr_->set_disconnected_callback(
            std::bind(
                    &EchoServer::DisconnectedCallback, this,
                    std::placeholders::_1,
                    std::placeholders::_2));

    server_uptr_->set_read_callback(
            std::bind(
                    &EchoServer::ReadCallback, this,
                    std::placeholders::_1,
                    std::placeholders::_2));

    server_uptr_->Start();
  }

 private:
  void ConnectedCallback(const cppbox::net::TcpConnectionSptr &tcp_conn_sptr, const cppbox::misc::SimpleTimeSptr &happened_st_sptr) {
    std::cout << "connected in thread " << cppbox::net::TcpConnectionThreadId() << std::endl;
    std::cout << "client ip " << tcp_conn_sptr->remote_ip() << std::endl;
    std::cout << "client port " << tcp_conn_sptr->remote_port() << std::endl;
    std::cout << "connected time " << happened_st_sptr->Format() << std::endl;

    tcp_conn_sptr->set_data_sptr(std::make_shared<MyTcpConnection>("new my tcp connection from echo server"));
  }

  void DisconnectedCallback(const cppbox::net::TcpConnectionSptr &tcp_conn_sptr, const cppbox::misc::SimpleTimeSptr &happened_st_sptr) {
    std::cout << "disconnected in thread " << cppbox::net::TcpConnectionThreadId() << std::endl;
    std::cout << "client ip " << tcp_conn_sptr->remote_ip() << std::endl;
    std::cout << "client port " << tcp_conn_sptr->remote_port() << std::endl;
    std::cout << "discnnected time " << happened_st_sptr->Format() << std::endl;
  }

  void ReadCallback(const cppbox::net::TcpConnectionSptr &tcp_conn_sptr, const cppbox::misc::SimpleTimeSptr &happened_st_sptr) {
    auto my_conn_sptr = std::static_pointer_cast<MyTcpConnection>(tcp_conn_sptr->data_sptr());

    std::cout << "in thread " << cppbox::net::TcpConnectionThreadId() << std::endl;
    std::cout << "connected time is " << tcp_conn_sptr->connected_time_sptr()->Format() << std::endl;
    std::cout << "last receive time is " << tcp_conn_sptr->last_receive_time_sptr()->Format() << std::endl;
    std::cout << "my_conn_msg is " << my_conn_sptr->msg() << std::endl;

    char buf[1024];
    tcp_conn_sptr->Send(buf, tcp_conn_sptr->Receive(buf, sizeof buf));
  }

  cppbox::net::TcpServerUptr server_uptr_;
};


TEST_F(TcpServerTest, EchoServer) {
  auto echo_server_uptr = std::unique_ptr<EchoServer>(new EchoServer(8860));

  echo_server_uptr->Start();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}