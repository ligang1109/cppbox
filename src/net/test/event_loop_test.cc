//
// Created by ligang on 19-1-7.
//

#include <thread>

#include "gtest/gtest.h"

#include "net/event_loop.h"

#include "misc/misc.h"

class EventLoopTest : public ::testing::Test {
 protected:
  EventLoopTest() {
  }

  ~EventLoopTest() override {
  }
};

void LoopThreadFunc(cppbox::net::EventLoop *event_loop_ptr, const std::string &name, int flag) {
  std::cout << "run loop " + name << std::endl;
  event_loop_ptr->Loop();
  std::cout << "quit loop " + name << std::endl;
}

TEST_F(EventLoopTest, Wakeup) {
  auto event_loop_ptr = new cppbox::net::EventLoop();
  event_loop_ptr->Init();

  std::thread t(LoopThreadFunc, event_loop_ptr, "wakeup", 0);
  sleep(2);

  std::cout << "wakeup loop" << std::endl;
  event_loop_ptr->Quit();

  t.join();

  delete event_loop_ptr;
}

TEST_F(EventLoopTest, Time) {
  auto event_loop_ptr = new cppbox::net::EventLoop();
  event_loop_ptr->Init();

  auto now_st_uptr = cppbox::misc::NowTimeUptr();
  auto te_sptr     = std::make_shared<cppbox::net::TimeEvent>();
  te_sptr->Init();

  time_t interval = 5;
  std::cout << "now time is " + now_st_uptr->Format() << std::endl;
  te_sptr->RunAt(now_st_uptr->Sec() + interval,
                 [event_loop_ptr](cppbox::misc::SimpleTimeSptr happened_st_sptr) {
                   std::cout << "run at time is " + happened_st_sptr->Format() << std::endl;
                   event_loop_ptr->Quit();
                 });

  event_loop_ptr->UpdateEvent(te_sptr);
  event_loop_ptr->Loop();

  now_st_uptr->Add(interval);
  std::cout << "now time is " + now_st_uptr->Format() << std::endl;
  te_sptr->RunAfter(interval,
                    [event_loop_ptr](cppbox::misc::SimpleTimeSptr happened_st_sptr) {
                      std::cout << "run after time is " + happened_st_sptr->Format() << std::endl;
                      event_loop_ptr->Quit();
                    });

  event_loop_ptr->Loop();

  int i = 0;
  now_st_uptr->Add(interval);
  std::cout << "now time is " + now_st_uptr->Format() << std::endl;
  te_sptr->RunEvery(interval,
                    [event_loop_ptr, &i](cppbox::misc::SimpleTimeSptr happened_st_sptr) {
                      std::cout << "run every time is " + happened_st_sptr->Format() << std::endl;
                      std::cout << "run every count is " << i << std::endl;

                      ++i;
                      if (i == 3) {
                        event_loop_ptr->Quit();
                      }
                    });

  event_loop_ptr->Loop();


  delete event_loop_ptr;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}