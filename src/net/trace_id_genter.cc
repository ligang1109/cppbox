//
// Created by ligang on 19-5-6.
//

#include "trace_id_genter.h"

#include "math.h"

namespace cppbox {

namespace net {

TraceIdGenter::TraceIdGenter(uint increment_len) :
        increment_(0),
        increment_len_(increment_len),
        max_increment_(static_cast<uint64_t >(::pow(10, increment_len))),
        increment_format_("%0" + std::to_string(increment_len) + "d"),
        increment_buf_(increment_len + 1) {}

std::string TraceIdGenter::GenId(const InetAddress &address) {
  std::string trace_id;

  auto start = 0;
  auto ip_len = address.ip.size();
  for (auto i = 0; i < ip_len; ++i) {
    if (address.ip[i] == '.') {
      auto len = i - start;
      std::string ip_part = address.ip.substr(start, len);
      start += len + 1;

      sprintf(ip_buf, "%02x", std::stoi(ip_part));
      trace_id.append(ip_buf, kIpHexPartLen);
    }
  }
  std::string ip_part = address.ip.substr(start, ip_len - start);
  sprintf(ip_buf, "%02x", std::stoi(ip_part));
  trace_id.append(ip_buf, kIpHexPartLen);

  sprintf(port_buf, "%05d", address.port);
  trace_id.append(port_buf, kPortLen);

  sprintf(increment_buf_.begin().base(), increment_format_.c_str(), increment_);
  trace_id.append(increment_buf_.begin().base(), increment_len_);

  increment_ = (increment_ + 1) % max_increment_;

  return trace_id;
}

}

}