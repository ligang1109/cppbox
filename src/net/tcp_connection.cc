//
// Created by ligang on 19-1-3.
//

#include "tcp_connection.h"

#include <unistd.h>
#include <sys/uio.h>
#include <sys/socket.h>

namespace cppbox {

namespace net {


TcpConnection::TcpConnection(int connfd, const InetAddress &address, EventLoop *loop_ptr, size_t read_protected_size) :
        connfd_(connfd),
        remote_ip_(address.ip),
        remote_port_(address.port),
        loop_ptr_(loop_ptr),
        status_(ConnectionStatus::kNotset),
        rw_event_sptr_(std::make_shared<Event>(connfd)),
        read_protected_size_(read_protected_size),
        read_buf_uptr_(new misc::SimpleBuffer()),
        write_buf_uptr_(new misc::SimpleBuffer()) {}

TcpConnection::~TcpConnection() {
  if (status_ != ConnectionStatus::kDisconnected) {
    loop_ptr_->DelEvent(connfd_);
    ::close(connfd_);
    status_ = ConnectionStatus::kDisconnected;
  }

  if (destruct_callback_ != nullptr) {
    destruct_callback_(*this);
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

EventLoop *TcpConnection::loop_ptr() {
  return loop_ptr_;
}

ConnectionStatus TcpConnection::status() {
  return status_;
}

misc::SimpleTimeSptr TcpConnection::connected_time_sptr() {
  return connected_time_sptr_;
}

misc::SimpleTimeSptr TcpConnection::last_receive_time_sptr() {
  return last_receive_time_sptr_;
}

void TcpConnection::set_connected_callback(const TcpConnCallback &cb) {
  connected_callback_ = cb;
}

void TcpConnection::set_disconnected_callback(const TcpConnCallback &cb) {
  disconnected_callback_ = cb;
}

void TcpConnection::set_read_callback(const TcpConnCallback &cb) {
  read_callback_ = cb;
}

void TcpConnection::set_write_complete_callback(const TcpConnCallback &cb) {
  write_complete_callback_ = cb;
}

void TcpConnection::set_error_callback(const TcpConnCallback &cb) {
  error_callback_ = cb;
}

void TcpConnection::set_destruct_callback(const DestructCallback &cb) {
  destruct_callback_ = cb;
}

void TcpConnection::ConnectEstablished(const misc::SimpleTimeSptr &happened_st_sptr) {
  status_ = ConnectionStatus::kConnected;
  connected_time_sptr_ = (happened_st_sptr == nullptr) ? misc::NowTimeSptr() : happened_st_sptr;

  rw_event_sptr_->set_events(Event::kReadEvents);
  rw_event_sptr_->set_read_callback(std::bind(&TcpConnection::ReadFdCallback, this, std::placeholders::_1));
  loop_ptr_->UpdateEvent(rw_event_sptr_);

  if (connected_callback_) {
    connected_callback_(shared_from_this(), connected_time_sptr_);
  }
}

void TcpConnection::GracefulClosed(const misc::SimpleTimeSptr &happened_st_sptr) {
  status_ = ConnectionStatus::kDisconnecting;

  if (::shutdown(connfd_, SHUT_RD) == -1) {
    ForceClosed(happened_st_sptr);
    return;
  }

  if (write_buf_uptr_->Readable() > 0) {
    return;
  }

  ForceClosed(happened_st_sptr);
}

void TcpConnection::ForceClosed(const misc::SimpleTimeSptr &happened_st_sptr) {
  status_ = ConnectionStatus::kDisconnected;

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

  if (n == len) {
    return len;
  }

  write_buf_uptr_->Append(data + n, len - n);
  EnsureWriteEvents();

  return n;
}


void TcpConnection::ReadFdCallback(const misc::SimpleTimeSptr &happened_st_sptr) {
  if (read_protected_size_ > 0) {
    if (read_buf_uptr_->Readable() >= read_protected_size_) {
      if (read_callback_) {
        read_callback_(shared_from_this(), happened_st_sptr);
      }
      return;
    }
  }

  char extrabuf[65536];
  auto writeable = read_buf_uptr_->Writeable();

  struct iovec iov[2];
  iov[0].iov_base = read_buf_uptr_->ReadBegin();
  iov[0].iov_len = writeable;
  iov[1].iov_base = extrabuf;
  iov[1].iov_len = sizeof extrabuf;

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

      ForceClosed();
      return;
    }

    if (n == 0) {
      GracefulClosed(happened_st_sptr);

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
  }
}

void TcpConnection::WriteFdCallback(const misc::SimpleTimeSptr &happened_st_sptr) {
  auto readable = write_buf_uptr_->Readable();
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

      ForceClosed();
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
  }

  if (write_buf_uptr_->Readable() > 0) {
    return;
  }

  if (status_ == ConnectionStatus::kDisconnecting) {
    ForceClosed();
    return;
  }

  rw_event_sptr_->DelEvents(Event::kWriteEvents);
  loop_ptr_->UpdateEvent(rw_event_sptr_);
}

void TcpConnection::EnsureWriteEvents() {
  if (!rw_event_sptr_->HasEvents(Event::kWriteEvents)) {
    rw_event_sptr_->set_events(Event::kWriteEvents);
    rw_event_sptr_->set_write_callback(std::bind(&TcpConnection::WriteFdCallback, this, std::placeholders::_1));
    loop_ptr_->UpdateEvent(rw_event_sptr_);
  }
}


}

}