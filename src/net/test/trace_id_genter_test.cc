//
// Created by ligang on 19-5-6.
//

#include "gtest/gtest.h"

#include "net/trace_id_genter.h"


class TraceIdGenterTest : public ::testing::Test {
 protected:
  TraceIdGenterTest() :
          tid_genter_uptr_(new cppbox::net::TraceIdGenter()) {}

  ~TraceIdGenterTest() override {}

  cppbox::net::TraceIdGenterUptr tid_genter_uptr_;

};

TEST_F(TraceIdGenterTest, GenId) {
  cppbox::net::InetAddress address = {"192.168.1.3", 8860};
  for (auto i = 0; i < 30000; ++i) {
    std::cout << tid_genter_uptr_->GenId(address) << std::endl;
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}