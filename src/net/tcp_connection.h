//
// Created by ligang on 19-1-3.
//

#ifndef CPPBOX_NET_TCP_CONNECTION_H
#define CPPBOX_NET_TCP_CONNECTION_H

#include <string>
#include <memory>

#include "event_loop.h"
#include "net.h"

#include "misc/non_copyable.h"
#include "misc/simple_buffer.h"
#include "misc/simple_time.h"

namespace cppbox {

namespace net {

enum class TcpConnectionStatus {
  kNotset        = 0,
  kDisconnected  = 1,
  kConnecting    = 2,
  kConnected     = 3,
  kGracefulClose = 4,
  kForceClose    = 5,
  kDisconnecting = 6,
};


class TcpConnection;

using TcpConnectionSptr = std::shared_ptr<TcpConnection>;
using TcpConnectionCallback = std::function<void(const TcpConnectionSptr &, const misc::SimpleTimeSptr &)>;

class TcpConnection : public misc::NonCopyable,
                      public std::enable_shared_from_this<TcpConnection> {
 public:
  using DestructCallback = std::function<void(TcpConnection &)>;
  using DataSptr = std::shared_ptr<void>;

  explicit TcpConnection(int connfd, const InetAddress &address, EventLoop *loop_ptr, size_t read_protected_size = 4096);

  ~TcpConnection();

  int connfd();

  std::string remote_ip();

  uint16_t remote_port();

  std::string trace_id();

  void set_trace_id(const std::string &trace_id);

  EventLoop *loop_ptr();

  TcpConnectionStatus status();

  misc::SimpleTimeSptr connected_time_sptr();

  misc::SimpleTimeSptr last_receive_time_sptr();

  void set_timeout_seconds(uint16_t timeout_seconds);

  uint16_t timeout_seconds();

  bool is_timeout();

  void set_connected_callback(const TcpConnectionCallback &cb);

  void set_disconnected_callback(const TcpConnectionCallback &cb);

  void set_read_callback(const TcpConnectionCallback &cb);

  void set_write_complete_callback(const TcpConnectionCallback &cb);

  void set_error_callback(const TcpConnectionCallback &cb);

  void set_data_sptr(const DataSptr &data_sptr);

  DataSptr data_sptr();

  void ConnectEstablished(const misc::SimpleTimeSptr &happened_st_sptr = nullptr);

  void Close(bool graceful = true, bool is_timeout = false);

  size_t Receive(char *data, size_t len);

  ssize_t Send(char *data, size_t len);

  ssize_t SendWriteBuffer();

  misc::SimpleBuffer *ReadBuffer();

  misc::SimpleBuffer *WriteBuffer();

  void Reset();

  void Reuse(int connfd, const InetAddress &address, EventLoop *loop_ptr);

 protected:
  void ReadFdCallback(const misc::SimpleTimeSptr &happened_st_sptr);

  void WriteFdCallback(const misc::SimpleTimeSptr &happened_st_sptr);

  void ErrorFdCallback(const misc::SimpleTimeSptr &happened_st_sptr);

  void EnsureWriteEvents();

  bool EnsureCloseAfterCallback();

  void GracefulClose(const misc::SimpleTimeSptr &happened_st_sptr = nullptr);

  void ForceClose(const misc::SimpleTimeSptr &happened_st_sptr = nullptr);

  int         connfd_;
  std::string remote_ip_;
  uint16_t    remote_port_;
  std::string trace_id_;

  EventLoop           *loop_ptr_;
  TcpConnectionStatus status_;
  EventSptr           rw_event_sptr_;
  size_t              read_protected_size_;

  misc::SimpleBufferUptr read_buf_uptr_;
  misc::SimpleBufferUptr write_buf_uptr_;

  misc::SimpleTimeSptr connected_time_sptr_;
  misc::SimpleTimeSptr last_receive_time_sptr_;
  uint16_t             timeout_seconds_;
  bool is_timeout_;

  TcpConnectionCallback connected_callback_;
  TcpConnectionCallback disconnected_callback_;
  TcpConnectionCallback read_callback_;
  TcpConnectionCallback write_complete_callback_;
  TcpConnectionCallback error_callback_;

  DataSptr data_sptr_;
};


}

}

#endif //CPPBOX_NET_TCP_CONNECTION_H
