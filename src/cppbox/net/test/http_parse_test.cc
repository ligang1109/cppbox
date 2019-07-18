//
// Created by ligang on 19-3-20.
//

#include "gtest/gtest.h"

#include "cppbox/net/http_parse.h"
#include "cppbox/net/http_request.h"
#include "cppbox/net/http_response.h"


class HttpParseTest : public ::testing::Test {
 protected:
  HttpParseTest() {
    parser_ptr_ = new cppbox::net::HttpParser(&pdata_);
  }

  ~HttpParseTest() override {
    delete parser_ptr_;
  }

  cppbox::net::HttpParseData pdata_;
  cppbox::net::HttpParser    *parser_ptr_;

  cppbox::net::HttpRequest  request_;
  cppbox::net::HttpResponse response_;
};

TEST_F(HttpParseTest, Request) {
  char raw_post1[] = "POST /demo/index?ab%3D1%26bcd%3D2%26cdef%3D3%26defgh%3Dasdfasdfsa HTTP/1.1\r\n"
                     "Host: www.abc.com\r\n"
                     "User-Agent: curl/7.18.0\r\n"
                     "Accept: */*\r\n"
                     "Content-Length: 5\r\n"
                     "Proxy: \r\n"
                     "Content-Type: application/x-www-form-urlencoded\r\n"
                     "\r\n"
                     "hello";

  auto len = strlen(raw_post1);
  auto n   = parser_ptr_->Execute(raw_post1, len);
  EXPECT_TRUE(n == len);

  char raw_post2[] = "POST /demo/index?ab%3D1%26bcd%3D2%26cdef%3D3%26defgh%3Dasdfasdfsa HTTP/1.1\r\n"
                     "Host: www.cppbox.com\r\n"
                     "User-Agent: curl/7.58.1\r\n"
                     "Accept: */*\r\n"
                     "Content-Length: 41\r\n"
                     "Content-Type: application/x-www-form-urlencoded\r\n"
                     "\r\n"
                     "name%3Dabc%26value%3Daabbcc%26desc%3Ddemo";

  pdata_.Reset();
  parser_ptr_->Reset();

  len = strlen(raw_post2);
  n   = parser_ptr_->Execute(raw_post2, len);
  EXPECT_TRUE(n == len);

  request_.ConvertFromData(std::move(pdata_));
  request_.ParseFormBody();

  cppbox::misc::SimpleBuffer sbuf;

  n = request_.AppendToBuffer(&sbuf);
  std::cout << n << std::endl;
  std::cout << sbuf.ReadAllAsString() << std::endl;

  std::cout << request_.HeaderValue("Accept") << std::endl;
  std::cout << request_.HeaderValue("accept") << std::endl;
}

TEST_F(HttpParseTest, Response) {
  char raw_response[] = "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/plain\r\n"
                        "Transfer-Encoding: chunked\r\n"
                        "\r\n"
                        "7\r\n"
                        "Mozilla\r\n"
                        "9\r\n"
                        "Developer\r\n"
                        "7\r\n"
                        "Network\r\n"
                        "0\r\n"
                        "\r\n";

  auto len = strlen(raw_response);
  auto n   = parser_ptr_->Execute(raw_response, len);
  EXPECT_TRUE(n == len);

  response_.ConvertFromData(std::move(pdata_));

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}