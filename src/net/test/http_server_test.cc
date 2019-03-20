//
// Created by ligang on 19-4-20.
//

#include "gtest/gtest.h"

#include "net/http_server.h"

class HttpServerTest : public ::testing::Test {
 protected:
  HttpServerTest() {
  }

  ~HttpServerTest() override {
  }
};


void DemoIndexFunc(const cppbox::net::HttpConnectionSptr &http_conn_sptr) {
  auto request_ptr = http_conn_sptr->Request();
  request_ptr->ParseFormBody();

  std::string body;
  body += "method=" + request_ptr->method() + "\n" +
          "raw_url=" + request_ptr->raw_url() + "\n";

  auto response_ptr = http_conn_sptr->Response();
  response_ptr->AddHeader("Response-by", "cppbox");
  response_ptr->set_body(body);

  http_conn_sptr->SendResponse();
}


TEST_F(HttpServerTest, DemoServer) {
  cppbox::net::HttpServer server(8860);
  auto err_uptr = server.Init(10, -1);
  if (err_uptr != nullptr) {
    std::cout << err_uptr->String() << std::endl;
    return;
  }

  server.AddHandleFunc("/demo/index", std::bind(DemoIndexFunc, std::placeholders::_1));
  err_uptr = server.Start();
  if (err_uptr != nullptr) {
    std::cout << err_uptr->String() << std::endl;
    return;
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}