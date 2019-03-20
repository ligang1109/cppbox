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

enum class ConnectionStatus {
  kNotset = 0,
  kDisconnected = 1,
  kConnecting = 2,
  kConnected = 3,
  kPrepareDisconnect = 4,
  kDisconnecting = 5,
};


class TcpConnection;

using TcpConnectionSptr = std::shared_ptr<TcpConnection>;
using TcpConnCallback = std::function<void(const TcpConnectionSptr &, const misc::SimpleTimeSptr &)>;

class TcpConnection : public misc::NonCopyable,
                      public std::enable_shared_from_this<TcpConnection> {
 public:
  using DestructCallback = std::function<void(TcpConnection &)>;

  explicit TcpConnection(int connfd, const InetAddress &address, EventLoop *loop_ptr, size_t read_protected_size = 4096);

  ~TcpConnection();

  int connfd();

  std::string remote_ip();

  uint16_t remote_port();

  EventLoop *loop_ptr();

  ConnectionStatus status();

  misc::SimpleTimeSptr connected_time_sptr();

  misc::SimpleTimeSptr last_receive_time_sptr();

  void set_connected_callback(const TcpConnCallback &cb);

  void set_disconnected_callback(const TcpConnCallback &cb);

  void set_read_callback(const TcpConnCallback &cb);

  void set_write_complete_callback(const TcpConnCallback &cb);

  void set_error_callback(const TcpConnCallback &cb);

  void set_destruct_callback(const DestructCallback &cb);

  void ConnectEstablished(const misc::SimpleTimeSptr &happened_st_sptr = nullptr);

  void Close();

  size_t Receive(char *data, size_t len);

  ssize_t Send(char *data, size_t len);

  ssize_t SendWriteBuffer();

  misc::SimpleBuffer *ReadBuffer();

  misc::SimpleBuffer *WriteBuffer();

 protected:
  void ReadFdCallback(const misc::SimpleTimeSptr &happened_st_sptr);

  void WriteFdCallback(const misc::SimpleTimeSptr &happened_st_sptr);

  void EnsureWriteEvents();

  bool EnsureCloseAfterCallback();

  void GracefulClose(const misc::SimpleTimeSptr &happened_st_sptr = nullptr);

  void ForceClose(const misc::SimpleTimeSptr &happened_st_sptr = nullptr);

  int connfd_;
  std::string remote_ip_;
  uint16_t remote_port_;
  EventLoop *loop_ptr_;
  ConnectionStatus status_;
  EventSptr rw_event_sptr_;
  size_t read_protected_size_;

  misc::SimpleBufferUptr read_buf_uptr_;
  misc::SimpleBufferUptr write_buf_uptr_;

  misc::SimpleTimeSptr connected_time_sptr_;
  misc::SimpleTimeSptr last_receive_time_sptr_;

  TcpConnCallback connected_callback_;
  TcpConnCallback disconnected_callback_;
  TcpConnCallback read_callback_;
  TcpConnCallback write_complete_callback_;
  TcpConnCallback error_callback_;

  DestructCallback destruct_callback_;
};


}

}

#endif //CPPBOX_NET_TCP_CONNECTION_H
