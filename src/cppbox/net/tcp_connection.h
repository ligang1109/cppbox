//
// Created by ligang on 19-1-3.
//

#ifndef CPPBOX_NET_TCP_CONNECTION_H
#define CPPBOX_NET_TCP_CONNECTION_H

#include <string>
#include <memory>

#include "event_loop.h"
#include "net.h"

#include "cppbox/misc/non_copyable.h"
#include "cppbox/misc/simple_buffer.h"
#include "cppbox/misc/simple_time.h"

namespace cppbox {

namespace net {


class TcpConnection;

using TcpConnectionSptr = std::shared_ptr<TcpConnection>;
using TcpConnectionCallback = std::function<void(const TcpConnectionSptr &, const misc::SimpleTimeSptr &)>;

class TcpConnection : public misc::NonCopyable,
                      public std::enable_shared_from_this<TcpConnection> {
 public:
  enum class ConnectionStatus {
    kNotset        = 0,
    kConnecting    = 1,
    kConnected     = 2,
    kDisconnecting = 3,
    kDisconnected  = 4,
    kTimeout       = 5,
    kError         = 6,
  };

  explicit TcpConnection(int connfd, const InetAddress &address, EventLoop *loop_ptr, size_t read_protected_size = 4096);

  virtual ~TcpConnection();

  int connfd();

  std::string remote_ip();

  uint16_t remote_port();

  std::string trace_id();

  void set_trace_id(const std::string &trace_id);

  EventLoop *loop_ptr();

  ConnectionStatus status();

  void set_status(ConnectionStatus status);

  misc::SimpleTimeSptr connected_time_sptr();

  misc::SimpleTimeSptr last_receive_time_sptr();

  misc::SimpleTimeSptr disconnected_time_sptr();

  void set_timeout_seconds(uint16_t timeout_seconds);

  uint16_t timeout_seconds();

  void set_connected_callback(const TcpConnectionCallback &cb);

  void set_disconnected_callback(const TcpConnectionCallback &cb);

  void set_read_callback(const TcpConnectionCallback &cb);

  void set_write_complete_callback(const TcpConnectionCallback &cb);

  void set_error_callback(const TcpConnectionCallback &cb);

  void set_timeout_callback(const TcpConnectionCallback &cb);

  void ConnectEstablished(const misc::SimpleTimeSptr &happen_st_sptr = nullptr);

  size_t Receive(char *data, size_t len);

  ssize_t Send(char *data, size_t len);

  ssize_t SendWriteBuffer();

  void GracefulClose(const misc::SimpleTimeSptr &happen_st_sptr = nullptr);

  void ForceClose(const misc::SimpleTimeSptr &happen_st_sptr = nullptr);

  misc::SimpleBuffer *ReadBuffer();

  misc::SimpleBuffer *WriteBuffer();

  void Reset();

  virtual void ResetMore();

  void Reuse(int connfd, const InetAddress &address, EventLoop *loop_ptr);

  void TimeoutCallback(const misc::SimpleTimeSptr &happen_st_sptr);

 protected:
  void ReadFdCallback(const misc::SimpleTimeSptr &happen_st_sptr);

  void WriteFdCallback(const misc::SimpleTimeSptr &happen_st_sptr);

  void ErrorFdCallback(const misc::SimpleTimeSptr &happen_st_sptr);

  void EnsureWriteEvents();

  int         connfd_;
  std::string remote_ip_;
  uint16_t    remote_port_;
  std::string trace_id_;

  EventLoop        *loop_ptr_;
  ConnectionStatus status_;
  EventSptr        rw_event_sptr_;
  size_t           read_protected_size_;

  misc::SimpleBufferUptr read_buf_uptr_;
  misc::SimpleBufferUptr write_buf_uptr_;

  misc::SimpleTimeSptr connected_time_sptr_;
  misc::SimpleTimeSptr last_receive_time_sptr_;
  misc::SimpleTimeSptr disconnected_time_sptr_;
  uint16_t             timeout_seconds_;

  TcpConnectionCallback connected_callback_;
  TcpConnectionCallback disconnected_callback_;
  TcpConnectionCallback read_callback_;
  TcpConnectionCallback write_complete_callback_;
  TcpConnectionCallback error_callback_;
  TcpConnectionCallback timeout_callback_;
};


}

}

#endif //CPPBOX_NET_TCP_CONNECTION_H
