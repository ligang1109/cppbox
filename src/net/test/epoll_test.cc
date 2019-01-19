//
// Created by ligang on 19-1-4.
//

#include "gtest/gtest.h"

#include "net/epoll.h"
#include "net/net.h"

class EpollTest : public ::testing::Test {
 protected:
  void SetUp() override {
    epoll_ = new cppbox::net::Epoll();

    sockfd_ = cppbox::net::NewTcpIpV4NonBlockSocket();
    cppbox::net::BindAndListenForTcpIpV4(sockfd_, "127.0.0.1", 8860);
  }

  void TearDown() override {
    delete epoll_;
    ::close(sockfd_);
  }

  cppbox::net::Epoll *epoll_;

  int sockfd_;
};

TEST_F(EpollTest, Epoll) {
  auto error_uptr = epoll_->Init();
  if (error_uptr != nullptr) {
    std::cout << "epoll create error: " + error_uptr->String() << std::endl;
    return;
  }

  error_uptr = epoll_->Add(sockfd_, EPOLLIN);
  if (error_uptr != nullptr) {
    std::cout << "epoll add error: " + error_uptr->String() << std::endl;
    return;
  }

  cppbox::net::Epoll::ReadyList ready_list;
  error_uptr = epoll_->Wait(&ready_list, -1);
  if (error_uptr != nullptr) {
    std::cout << "epoll wait error: " + error_uptr->String() << std::endl;
    return;
  }

  if (ready_list.empty()) {
    std::cout << "no fd ready" << std::endl;
    return;
  }

  for (auto &ri :ready_list) {
    std::cout << "fd: "
              << ri.first
              << ", events: "
              << ri.second
              << std::endl;
    epoll_->PrintEvents(ri.second);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}