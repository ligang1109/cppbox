//
// Created by ligang on 19-5-6.
//

#ifndef CPPBOX_NET_TRACE_ID_GENTER_H
#define CPPBOX_NET_TRACE_ID_GENTER_H


#include <string>
#include <vector>

#include "net.h"


namespace cppbox {

namespace net {

class TraceIdGenter {
 public:
  explicit TraceIdGenter(uint increment_len = 4);

  std::string GenId(const InetAddress &address);

 private:
  static const uint kIpHexPartLen = 2;
  static const uint kPortLen = 5;


  uint increment_;
  uint increment_len_;
  u_int64_t max_increment_;
  std::string increment_format_;

  char ip_buf[kIpHexPartLen + 1];
  char port_buf[kPortLen + 1];
  std::vector<char> increment_buf_;
};

using TraceIdGenterUptr = std::unique_ptr<TraceIdGenter>;


}

}


#endif //CPPBOX_NET_TRACE_ID_GENTER_H
