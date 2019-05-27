//
// Created by ligang on 19-1-3.
//

#include "tcp_connection.h"

#include <unistd.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <log/base.h>

namespace cppbox {

namespace net {


TcpConnection::TcpConnection(int connfd, const InetAddress &address, EventLoop *loop_ptr, size_t read_protected_size) :
        connfd_(connfd),
        remote_ip_(address.ip),
        remote_port_(address.port),
        loop_ptr_(loop_ptr),
        status_(TcpConnectionStatus::kNotset),
        rw_event_sptr_(new Event(connfd_)),
        read_protected_size_(read_protected_size),
        read_buf_uptr_(new misc::SimpleBuffer()),
        write_buf_uptr_(new misc::SimpleBuffer()),
        timeout_seconds_(0),
        is_timeout_(false),
        data_sptr_(nullptr) {}

TcpConnection::~TcpConnection() {
  if (status_ != TcpConnectionStatus::kDisconnected && status_ != TcpConnectionStatus::kNotset) {
    loop_ptr_->DelEvent(connfd_);
    ::close(connfd_);
    status_ = TcpConnectionStatus::kDisconnected;
  }
}

int TcpConnection::connfd() {
  return connfd_;
}

std::string TcpConnection::remote_ip() {
  return remote_ip_;
}

uint16_t TcpConnection::remote_port() {
  return remote_port_;
}

std::string TcpConnection::trace_id() {
  return trace_id_;
}

void TcpConnection::set_trace_id(const std::string &trace_id) {
  trace_id_ = trace_id;
}

EventLoop *TcpConnection::loop_ptr() {
  return loop_ptr_;
}

TcpConnectionStatus TcpConnection::status() {
  return status_;
}

misc::SimpleTimeSptr TcpConnection::connected_time_sptr() {
  return connected_time_sptr_;
}

misc::SimpleTimeSptr TcpConnection::last_receive_time_sptr() {
  return last_receive_time_sptr_;
}

void TcpConnection::set_timeout_seconds(uint16_t timeout_seconds) {
  timeout_seconds_ = timeout_seconds;
}

uint16_t TcpConnection::timeout_seconds() {
  return timeout_seconds_;
}

bool TcpConnection::is_timeout() {
  return is_timeout_;
}

void TcpConnection::set_connected_callback(const TcpConnectionCallback &cb) {
  connected_callback_ = cb;
}

void TcpConnection::set_disconnected_callback(const TcpConnectionCallback &cb) {
  disconnected_callback_ = cb;
}

void TcpConnection::set_read_callback(const TcpConnectionCallback &cb) {
  read_callback_ = cb;
}

void TcpConnection::set_write_complete_callback(const TcpConnectionCallback &cb) {
  write_complete_callback_ = cb;
}

void TcpConnection::set_error_callback(const TcpConnectionCallback &cb) {
  error_callback_ = cb;
}

void TcpConnection::set_data_sptr(const TcpConnection::DataSptr &data_sptr) {
  data_sptr_ = data_sptr;
}

TcpConnection::DataSptr TcpConnection::data_sptr() {
  return data_sptr_;
}

void TcpConnection::ConnectEstablished(const misc::SimpleTimeSptr &happened_st_sptr) {
  status_              = TcpConnectionStatus::kConnected;
  connected_time_sptr_ = (happened_st_sptr == nullptr) ? misc::NowTimeSptr() : happened_st_sptr;

  rw_event_sptr_->set_events(Event::kReadEvents | Event::kErrorEvents);
  rw_event_sptr_->set_read_callback(std::bind(&TcpConnection::ReadFdCallback, this, std::placeholders::_1));
  rw_event_sptr_->set_error_callback(std::bind(&TcpConnection::ErrorFdCallback, this, std::placeholders::_1));
  loop_ptr_->UpdateEvent(rw_event_sptr_);

  if (connected_callback_) {
    connected_callback_(shared_from_this(), connected_time_sptr_);
  }
}

void TcpConnection::Close(bool graceful, bool is_timeout) {
  if (graceful) {
    status_ = TcpConnectionStatus::kGracefulClose;
  } else {
    status_ = TcpConnectionStatus::kForceClose;
  }

  is_timeout_ = is_timeout;
}


size_t TcpConnection::Receive(char *data, size_t len) {
  return read_buf_uptr_->Read(data, len);
}

ssize_t TcpConnection::Send(char *data, size_t len) {
  if (write_buf_uptr_->Readable() > 0) {
    write_buf_uptr_->Append(data, len);
    EnsureWriteEvents();

    return 0;
  }

  ssize_t n;
  while (true) {
    n = ::write(connfd_, data, len);
    if (n == -1) {
      if (errno == EINTR) {
        continue;
      }

      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        write_buf_uptr_->Append(data, len);
        EnsureWriteEvents();

        return 0;
      }

      return -1;
    }
    break;
  }

  if (n < len) {
    write_buf_uptr_->Append(data + n, len - n);
    EnsureWriteEvents();
  }

  return n;
}

ssize_t TcpConnection::SendWriteBuffer() {
  auto readable = write_buf_uptr_->Readable();
  if (readable == 0) {
    return 0;
  }

  ssize_t n;
  while (true) {
    n = ::write(connfd_, write_buf_uptr_->ReadBegin(), readable);
    if (n == -1) {
      if (errno == EINTR) {
        continue;
      }

      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        EnsureWriteEvents();

        return 0;
      }

      return -1;
    }
    break;
  }

  write_buf_uptr_->AddReadIndex(static_cast<size_t>(n));

  if (n < readable) {
    EnsureWriteEvents();
  }

  return n;
}

misc::SimpleBuffer *TcpConnection::ReadBuffer() {
  return read_buf_uptr_.get();
}

misc::SimpleBuffer *TcpConnection::WriteBuffer() {
  return write_buf_uptr_.get();
}

void TcpConnection::Reset() {
  connfd_ = 0;
  remote_ip_.clear();
  remote_port_ = 0;
  trace_id_.clear();

  loop_ptr_ = nullptr;
  status_   = TcpConnectionStatus::kNotset;
  rw_event_sptr_->Reset();

  read_buf_uptr_->Reset();
  write_buf_uptr_->Reset();

  timeout_seconds_ = 0;

  connected_callback_      = nullptr;
  disconnected_callback_   = nullptr;
  read_callback_           = nullptr;
  write_complete_callback_ = nullptr;
  error_callback_          = nullptr;
}

void TcpConnection::Reuse(int connfd, const InetAddress &address, EventLoop *loop_ptr) {
  connfd_ = connfd;
  rw_event_sptr_->set_fd(connfd);

  remote_ip_   = address.ip;
  remote_port_ = address.port;
  loop_ptr_    = loop_ptr;
}


void TcpConnection::ReadFdCallback(const misc::SimpleTimeSptr &happened_st_sptr) {
  if (read_protected_size_ > 0) {
    if (read_buf_uptr_->Readable() >= read_protected_size_) {
      if (read_callback_) {
        read_callback_(shared_from_this(), happened_st_sptr);
        EnsureCloseAfterCallback();
      }
      return;
    }
  }

  char extrabuf[65536];
  auto writeable = read_buf_uptr_->Writeable();

  struct iovec iov[2];
  iov[0].iov_base = read_buf_uptr_->WriteBegin();
  iov[0].iov_len  = writeable;
  iov[1].iov_base = extrabuf;
  iov[1].iov_len  = sizeof extrabuf;


  ssize_t n;
  while (true) {
    n = ::readv(connfd_, iov, 2);
    if (n == -1) {
      if (errno == EINTR) {
        continue;
      }

      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return;
      }

      if (error_callback_) {
        error_callback_(shared_from_this(), happened_st_sptr);
      }

      ForceClose(happened_st_sptr);
      return;
    }

    if (n == 0) {
      GracefulClose(happened_st_sptr);
      return;
    }
    break;
  }

  last_receive_time_sptr_ = happened_st_sptr;

  if (n <= writeable) {
    read_buf_uptr_->AddWriteIndex(static_cast<size_t>(n));
  } else {
    read_buf_uptr_->AddWriteIndex(writeable);
    read_buf_uptr_->Append(extrabuf, n - writeable);
  }

  if (read_callback_) {
    read_callback_(shared_from_this(), happened_st_sptr);
    EnsureCloseAfterCallback();
  }
}

void TcpConnection::WriteFdCallback(const misc::SimpleTimeSptr &happened_st_sptr) {
  auto readable = write_buf_uptr_->Readable();
  if (readable > 0) {
    ssize_t n;
    while (true) {
      n = ::write(connfd_, write_buf_uptr_->ReadBegin(), readable);
      if (n == -1) {
        if (errno == EINTR) {
          continue;
        }

        if (errno == EAGAIN || errno == EWOULDBLOCK) {
          return;
        }

        if (error_callback_) {
          error_callback_(shared_from_this(), happened_st_sptr);
        }

        ForceClose(happened_st_sptr);
        return;
      }
      break;
    }

    write_buf_uptr_->AddReadIndex(static_cast<size_t>(n));

    if (n < readable) {
      return;
    }

    if (write_complete_callback_) {
      write_complete_callback_(shared_from_this(), happened_st_sptr);
      if (EnsureCloseAfterCallback()) {
        return;
      }
    }

    if (write_buf_uptr_->Readable() > 0) {
      return;
    }
  }


  if (status_ == TcpConnectionStatus::kDisconnecting) {
    ForceClose();
    return;
  }

  rw_event_sptr_->DelEvents(Event::kWriteEvents);
  loop_ptr_->UpdateEvent(rw_event_sptr_);
}

void TcpConnection::ErrorFdCallback(const misc::SimpleTimeSptr &happened_st_sptr) {
  if (error_callback_) {
    error_callback_(shared_from_this(), happened_st_sptr);
  }

  ForceClose(happened_st_sptr);
}

void TcpConnection::EnsureWriteEvents() {
  if (!rw_event_sptr_->HasEvents(Event::kWriteEvents)) {
    rw_event_sptr_->AddEvents(Event::kWriteEvents);
    rw_event_sptr_->set_write_callback(std::bind(&TcpConnection::WriteFdCallback, this, std::placeholders::_1));
    loop_ptr_->UpdateEvent(rw_event_sptr_);
  }
}

bool TcpConnection::EnsureCloseAfterCallback() {
  switch (status_) {
    case TcpConnectionStatus::kGracefulClose:
      GracefulClose();
      return true;
    case TcpConnectionStatus::kForceClose:
      ForceClose();
      return true;
    default:
      return false;
  }
}

void TcpConnection::GracefulClose(const misc::SimpleTimeSptr &happened_st_sptr) {
  status_ = TcpConnectionStatus::kDisconnecting;

  rw_event_sptr_->DelEvents(Event::kReadEvents);
  loop_ptr_->UpdateEvent(rw_event_sptr_);
  if (::shutdown(connfd_, SHUT_RD) == -1) {
    ForceClose(happened_st_sptr);
    return;
  }

  if (write_buf_uptr_->Readable() > 0) {
    EnsureWriteEvents();
    return;
  }

  ForceClose(happened_st_sptr);
}

void TcpConnection::ForceClose(const misc::SimpleTimeSptr &happened_st_sptr) {
  status_ = TcpConnectionStatus::kDisconnected;

  loop_ptr_->DelEvent(connfd_);
  ::close(connfd_);

  if (disconnected_callback_) {
    if (happened_st_sptr == nullptr) {
      disconnected_callback_(shared_from_this(), misc::NowTimeSptr());
    } else {
      disconnected_callback_(shared_from_this(), happened_st_sptr);
    }
  }
}


}

}