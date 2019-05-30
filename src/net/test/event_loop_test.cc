//
// Created by ligang on 19-1-7.
//

#include <thread>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "gtest/gtest.h"

#include "net/event_loop.h"
#include "net/net.h"

#include "misc/misc.h"
#include "misc/simple_buffer.h"

class EventLoopTest : public ::testing::Test {
 protected:
  EventLoopTest() {
    event_loop_ptr_ = new cppbox::net::EventLoop();
    EXPECT_TRUE(event_loop_ptr_->Init() == nullptr);
  }

  ~EventLoopTest() override {
    delete event_loop_ptr_;
  }

  cppbox::net::EventLoop *event_loop_ptr_;
};

void LoopThreadFunc(cppbox::net::EventLoop *event_loop_ptr, const std::string &name, int flag) {
  std::cout << "run loop " + name << std::endl;
  event_loop_ptr->Loop();
  std::cout << "quit loop " + name << std::endl;
}

TEST_F(EventLoopTest, Wakeup) {
  std::thread t(LoopThreadFunc, event_loop_ptr_, "wakeup", 0);
  sleep(1);

  std::cout << "wakeup loop" << std::endl;
  event_loop_ptr_->Quit();

  t.join();
}

void AppendFunc() {
  std::cout << "run append func" << std::endl;
}

TEST_F(EventLoopTest, AppenFunction) {
  std::thread t(LoopThreadFunc, event_loop_ptr_, "append function", 0);
  sleep(1);

  for (int i = 0; i < 3; ++i) {
    event_loop_ptr_->AppendFunction(std::bind(AppendFunc));
  }
  event_loop_ptr_->Quit();

  t.join();
}

TEST_F(EventLoopTest, Time) {
  auto event_loop_ptr = event_loop_ptr_;

  auto now_st_uptr = cppbox::misc::NowTimeUptr();
  auto te_sptr = std::make_shared<cppbox::net::TimeEvent>();
  te_sptr->Init();

  time_t interval = 5;
  std::cout << "now time is " + now_st_uptr->Format() << std::endl;
  te_sptr->RunAt(now_st_uptr->Sec() + interval,
                 [event_loop_ptr](cppbox::misc::SimpleTimeSptr happen_st_sptr) {
                   std::cout << "run at time is " + happen_st_sptr->Format() << std::endl;
                   event_loop_ptr->Quit();
                 });

  event_loop_ptr->UpdateEvent(te_sptr);
  event_loop_ptr->Loop();

  now_st_uptr->Add(interval);
  std::cout << "now time is " + now_st_uptr->Format() << std::endl;
  te_sptr->RunAfter(interval,
                    [event_loop_ptr](cppbox::misc::SimpleTimeSptr happen_st_sptr) {
                      std::cout << "run after time is " + happen_st_sptr->Format() << std::endl;
                      event_loop_ptr->Quit();
                    });

  event_loop_ptr->Loop();

  int i = 0;
  now_st_uptr->Add(interval);
  std::cout << "now time is " + now_st_uptr->Format() << std::endl;
  te_sptr->RunEvery(interval,
                    [event_loop_ptr, &i](cppbox::misc::SimpleTimeSptr happen_st_sptr) {
                      std::cout << "run every time is " + happen_st_sptr->Format() << std::endl;
                      std::cout << "run every count is " << i << std::endl;

                      ++i;
                      if (i == 3) {
                        event_loop_ptr->Quit();
                      }
                    });

  event_loop_ptr->Loop();
}

TEST_F(EventLoopTest, RW) {
  auto event_loop_ptr = event_loop_ptr_;

  int sockfd = cppbox::net::NewTcpIpV4NonBlockSocket();
  cppbox::net::BindAndListenForTcpIpV4(sockfd, "127.0.0.1", 8860);

  auto sbuf_ptr = new cppbox::misc::SimpleBuffer(100);
  auto event_sptr = std::make_shared<cppbox::net::Event>(sockfd);

  event_sptr->set_events(cppbox::net::Event::kReadEvents);
  event_sptr->set_read_callback(
          [event_loop_ptr, sockfd, sbuf_ptr](cppbox::misc::SimpleTimeSptr happen_st_sptr) {
            struct sockaddr_in clientAddr;
            memset(&clientAddr, 0, sizeof(struct sockaddr_in));
            socklen_t clientLen = 1;

            int connfd = ::accept4(sockfd, (struct sockaddr *) &clientAddr, &clientLen, SOCK_CLOEXEC | SOCK_NONBLOCK);
            getpeername(connfd, (struct sockaddr *) &clientAddr, &clientLen);
            char *ip = inet_ntoa(clientAddr.sin_addr);
            uint16_t port = ntohs(clientAddr.sin_port);
            std::cout << ip << ":" << port << std::endl;

            auto cevent_sptr = std::make_shared<cppbox::net::Event>(connfd);
            cevent_sptr->set_events(cppbox::net::Event::kReadEvents);
            cevent_sptr->set_read_callback(
                    [event_loop_ptr, connfd, sbuf_ptr](cppbox::misc::SimpleTimeSptr happen_st_sptr) {
                      char buf[100];
                      auto n = ::read(connfd, buf, sizeof(buf));
                      if (n == 0) {
                        std::cout << "client disconnect" << std::endl;
                        event_loop_ptr->DelEvent(connfd);
                        ::close(connfd);
                        return;
                      }

                      sbuf_ptr->Append(buf, n);

                      auto cevent_sptr = event_loop_ptr->GetEvent(connfd);
                      cevent_sptr->set_events(cppbox::net::Event::kReadEvents | cppbox::net::Event::kWriteEvents);
                      event_loop_ptr->UpdateEvent(cevent_sptr);
                    });
            cevent_sptr->set_write_callback(
                    [event_loop_ptr, connfd, sbuf_ptr](cppbox::misc::SimpleTimeSptr happen_st_sptr) {
                      auto s = sbuf_ptr->ReadAllAsString();
                      std::cout << "send " + s << std::endl;
                      ::write(connfd, s.c_str(), s.size());
                      sbuf_ptr->Reset();

                      auto cevent_sptr = event_loop_ptr->GetEvent(connfd);
                      cevent_sptr->set_events(cppbox::net::Event::kReadEvents);
                      event_loop_ptr->UpdateEvent(cevent_sptr);
                    });

            event_loop_ptr->UpdateEvent(cevent_sptr);
          });

  event_loop_ptr->UpdateEvent(event_sptr);
  event_loop_ptr->Loop();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}