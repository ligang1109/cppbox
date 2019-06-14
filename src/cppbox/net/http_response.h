//
// Created by ligang on 19-3-20.
//

#ifndef CPPBOX_NET_HTTP_RESPONSE_H
#define CPPBOX_NET_HTTP_RESPONSE_H

#include <string>
#include <map>

#include "cppbox/misc/simple_buffer.h"
#include "cppbox/misc/non_copyable.h"


namespace cppbox {

namespace net {


class HttpResponse : public misc::NonCopyable {
 public:
  HttpResponse();

  void set_body(const std::string &body);

  void SetStatus(int code, const std::string &msg);

  void AddHeader(const std::string &field, const std::string &value);

  size_t AppendToBuffer(misc::SimpleBuffer *sbuf_ptr);

  void Reset();

 private:
  int         status_code_;
  std::string status_msg_;
  std::string body_;

  std::map<std::string, std::string> header_map_;
};

using HttpResponseUptr = std::unique_ptr<HttpResponse>;


}

}


#endif //CPPBOX_NET_HTTP_RESPONSE_H
