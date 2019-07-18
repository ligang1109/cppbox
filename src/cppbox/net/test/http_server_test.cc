//
// Created by ligang on 19-4-20.
//

#include "gtest/gtest.h"

#include "cppbox/net/http_server.h"

#include "cppbox/log/base.h"
#include "cppbox/log/simple_formater.h"
#include "cppbox/log/simple_logger.h"
#include "cppbox/log/file_writer.h"
#include "cppbox/log/async_writer.h"
#include "cppbox/log/console_writer.h"


cppbox::log::SimpleLogger *access_logger;

class HttpServerTest : public ::testing::Test {
 protected:
  HttpServerTest() {
    cppbox::log::FormaterSptr fr  = std::make_shared<cppbox::log::SimpleFormater>();
    cppbox::log::WriterSptr   afw = std::make_shared<cppbox::log::FileWriter>("/tmp/cppbox_net_http_server_test.log", 0);
    cppbox::log::WriterSptr   aw  = std::make_shared<cppbox::log::AsyncWriter>(afw);
    access_logger = new cppbox::log::SimpleLogger(aw, fr, cppbox::log::LogLevel::kDEBUG);
  }

  ~HttpServerTest() override {
  }
};


void DemoIndexFunc(const cppbox::net::HttpConnectionSptr &http_conn_sptr) {
  auto request_ptr  = http_conn_sptr->Request();
  auto response_ptr = http_conn_sptr->Response();

  request_ptr->ParseFormBody();

  std::string body;
  body += "method=" + request_ptr->method() + "\n" +
          "raw_url=" + request_ptr->raw_url() + "\n";

  response_ptr->AddHeader("Response-by", "cppbox");
  response_ptr->set_body(body);

  http_conn_sptr->SendResponse();

  access_logger->Info("raw_url=" + request_ptr->raw_url());
}


TEST_F(HttpServerTest, DemoServer) {
  cppbox::net::HttpServer server(8860);

  auto err_uptr = server.Init(10, -1);
  if (err_uptr != nullptr) {
    std::cout << err_uptr->String() << std::endl;
    return;
  }

  server.AddHandleFunc("/demo/index", std::bind(
          DemoIndexFunc,
          std::placeholders::_1));

  server.SetLogger(access_logger);

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