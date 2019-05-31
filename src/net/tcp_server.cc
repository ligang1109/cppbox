//
// Created by ligang on 19-2-3.
//

#include "tcp_server.h"

#include "misc/misc.h"


namespace cppbox {

namespace net {


__thread int tcp_conn_thread_id;

int TcpConnectionThreadId() {
  return tcp_conn_thread_id;
}

TcpServer::TcpServer(uint16_t port, const std::string &ip, uint16_t default_conn_idle_seconds) :
        ip_(ip),
        port_(port),
        logger_ptr_(nullptr),
        trace_id_genter_uptr_(new TraceIdGenter()),
        loop_uptr_(nullptr),
        dispatch_index_(0),
        default_conn_idle_seconds_(default_conn_idle_seconds) {}

std::string TcpServer::ip() {
  return ip_;
}

uint16_t TcpServer::port() {
  return port_;
}

void TcpServer::set_logger_ptr(log::LoggerInterface *logger_ptr) {
  logger_ptr_ = logger_ptr;
}

void TcpServer::set_connected_callback(const TcpConnectionCallback &cb) {
  connected_callback_ = cb;
}

void TcpServer::set_disconnected_callback(const TcpConnectionCallback &cb) {
  disconnected_callback_ = cb;
}

void TcpServer::set_read_callback(const TcpConnectionCallback &cb) {
  read_callback_ = cb;
}

void TcpServer::set_write_complete_callback(const TcpConnectionCallback &cb) {
  write_complete_callback_ = cb;
}

void TcpServer::set_error_callback(const TcpConnectionCallback &cb) {
  error_callback_ = cb;
}

misc::ErrorUptr TcpServer::Init(int thread_cnt, int loop_timeout_ms, int init_evlist_size, size_t tcp_conn_pool_shard_size, size_t tcp_conn_pool_max_shard_cnt) {
  if (default_conn_idle_seconds_ > TcpConnectionTimeWheel::kMaxConnIdleSeconds) {
    return misc::NewErrorUptr(misc::Error::kInvalidArg, "default_conn_idle_seconds > max_conn_idle_seconds");
  }

  listenfd_ = NewTcpIpV4NonBlockSocket();
  if (listenfd_ == -1) {
    return misc::NewErrorUptrByErrno();
  }

  loop_uptr_.reset(new EventLoop());
  auto err_uptr = loop_uptr_->Init(init_evlist_size);
  if (err_uptr != nullptr) {
    return err_uptr;
  }

  for (auto i = 0; i < thread_cnt; ++i) {
    auto conn_thread_uptr = misc::MakeUnique<ConnectionThread>(i, this, tcp_conn_pool_shard_size, tcp_conn_pool_max_shard_cnt);
    err_uptr = conn_thread_uptr->Init(loop_timeout_ms, init_evlist_size);
    if (err_uptr != nullptr) {
      return err_uptr;
    }

    conn_thread_list_.push_back(std::move(conn_thread_uptr));
  }

  return nullptr;
}


misc::ErrorUptr TcpServer::Start() {
  if (logger_ptr_ == nullptr) {
    logger_ptr_ = new log::NullLogger();
  }

  logger_ptr_->Debug("TcpServer::Start");

  for (auto &conn_thread_uptr : conn_thread_list_) {
    conn_thread_uptr->Start();
  }

  return ListenAndServe();
}

size_t TcpServer::ConnectionCount() {
  size_t    cnt = 0;
  for (auto &conn_thread_uptr : conn_thread_list_) {
    cnt += conn_thread_uptr->ConnectionCount();
  }

  return cnt;
}

void TcpServer::RunFunctionInConnectionThread(int conn_thread_id, const EventLoop::Functor &func) {
  if (conn_thread_id < conn_thread_list_.size()) {
    conn_thread_list_[conn_thread_id]->RunFunction(func);
  }
}

misc::ErrorUptr TcpServer::ListenAndServe() {
  auto err_uptr = BindAndListenForTcpIpV4(listenfd_, ip_.c_str(), port_);
  if (err_uptr != nullptr) {
    return err_uptr;
  }

  auto event_sptr = std::make_shared<cppbox::net::Event>(listenfd_);
  event_sptr->set_events(Event::kReadEvents);
  event_sptr->set_read_callback(std::bind(&TcpServer::ListenCallback, this, std::placeholders::_1));

  loop_uptr_->UpdateEvent(event_sptr);
  loop_uptr_->Loop();

  return nullptr;
}

void TcpServer::ListenCallback(const misc::SimpleTimeSptr &happen_st_sptr) {
  InetAddress raddr;

  int connfd;
  while (true) {
    connfd = Accept(listenfd_, raddr);
    if (connfd == -1) {
      if (errno == EINTR) {
        continue;
      }

      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return;
      }

      logger_ptr_->Error("accept error: " + cppbox::misc::NewErrorUptrByErrno()->String());
      return;
    }

    break;
  }

  conn_thread_list_[dispatch_index_]->AddConnection(connfd, raddr, happen_st_sptr, trace_id_genter_uptr_->GenId(raddr));
  dispatch_index_ = (dispatch_index_ + 1) % conn_thread_list_.size();
}


TcpServer::ConnectionThread::ConnectionThread(int id, TcpServer *server_ptr, size_t tcp_conn_pool_shard_size, size_t tcp_conn_pool_max_shard_cnt) :
        id_(id),
        server_ptr_(server_ptr),
        thread_uptr_(nullptr),
        loop_uptr_(nullptr),
        time_wheel_uptr_(nullptr),
        pool_uptr_(new TcpConnectionPool(tcp_conn_pool_shard_size, tcp_conn_pool_max_shard_cnt)) {}

misc::ErrorUptr TcpServer::ConnectionThread::Init(int loop_timeout_ms, int init_evlist_size) {
  loop_uptr_.reset(new EventLoop(loop_timeout_ms));
  auto err_uptr = loop_uptr_->Init(init_evlist_size);
  if (err_uptr != nullptr) {
    return err_uptr;
  }

  time_wheel_uptr_.reset(new TcpConnectionTimeWheel(loop_uptr_.get(),
                                                    std::bind(&TcpServer::ConnectionThread::TimeoutCallback,
                                                              this,
                                                              std::placeholders::_1,
                                                              std::placeholders::_2)));

  if (server_ptr_->default_conn_idle_seconds_ > 0) {
    err_uptr = time_wheel_uptr_->Init();
    if (err_uptr != nullptr) {
      return err_uptr;
    }
  }

  auto        size = pool_uptr_->shard_size();
  InetAddress address;

  for (auto i = 0; i < size; ++i) {
    pool_uptr_->Put(std::make_shared<TcpConnection>(0, address, nullptr));
  }

  return nullptr;
}

void TcpServer::ConnectionThread::Start() {
  thread_uptr_.reset(new std::thread(
          std::bind(&TcpServer::ConnectionThread::ThreadFunc, this)
                                    ));
}

void TcpServer::ConnectionThread::AddConnection(int connfd, const InetAddress &remote_addr, const misc::SimpleTimeSptr &happen_st_sptr, const std::string &trace_id) {
  loop_uptr_->AppendFunction(
          std::bind(&TcpServer::ConnectionThread::AddConnectionInThread, this, connfd, remote_addr, happen_st_sptr, trace_id)
                            );
}

size_t TcpServer::ConnectionThread::ConnectionCount() {
  return conn_time_hand_map_.size();
}

void TcpServer::ConnectionThread::RunFunction(const EventLoop::Functor &func) {
  loop_uptr_->AppendFunction(func);
}

void TcpServer::ConnectionThread::UpdateActiveConnection(const TcpConnectionSptr &tcp_conn_sptr) {
  auto connfd = tcp_conn_sptr->connfd();
  auto it     = conn_time_hand_map_.find(connfd);
  if (it == conn_time_hand_map_.end()) {
    server_ptr_->logger_ptr_->Error("tcp_conn is not in conn_time_hand_map");
    tcp_conn_sptr->ForceClose();
    return;
  }

  auto old_hand        = it->second;
  auto timeout_seconds = tcp_conn_sptr->timeout_seconds();
  if (timeout_seconds == 0) {
    timeout_seconds = server_ptr_->default_conn_idle_seconds_;
  }

  auto new_hand = time_wheel_uptr_->UpdateConnection(old_hand, connfd, timeout_seconds);
  if (new_hand == TcpConnectionTimeWheel::kWheelSize) {
    server_ptr_->logger_ptr_->Error("tcp_conn is not in time_wheel");
    tcp_conn_sptr->ForceClose();
    return;
  }

  if (new_hand != old_hand) {
    it->second = new_hand;
  }
}

void TcpServer::ConnectionThread::DisconnectedCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happen_st_sptr) {
  if (server_ptr_->disconnected_callback_ != nullptr) {
    server_ptr_->disconnected_callback_(tcp_conn_sptr, happen_st_sptr);
  }

  auto connfd = tcp_conn_sptr->connfd();
  auto it     = conn_time_hand_map_.find(connfd);
  if (it != conn_time_hand_map_.end()) {
    time_wheel_uptr_->DelConnection(it->second, connfd);
    conn_time_hand_map_.erase(it);
  }

  if (pool_uptr_->Put(tcp_conn_sptr)) {
    tcp_conn_sptr->Reset();
  }
}

void TcpServer::ConnectionThread::TimeoutCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happen_st_sptr) {
  auto connfd = tcp_conn_sptr->connfd();
  auto it     = conn_time_hand_map_.find(connfd);
  if (it != conn_time_hand_map_.end()) {
    conn_time_hand_map_.erase(it);
  }

  tcp_conn_sptr->ForceClose(happen_st_sptr);
}

void TcpServer::ConnectionThread::ConnectionReadCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happen_st_sptr) {
  UpdateActiveConnection(tcp_conn_sptr);

  if (server_ptr_->read_callback_ != nullptr) {
    server_ptr_->read_callback_(tcp_conn_sptr, happen_st_sptr);
  }
}

void TcpServer::ConnectionThread::ThreadFunc() {
  tcp_conn_thread_id = id_;

  loop_uptr_->Loop();
}

void TcpServer::ConnectionThread::AddConnectionInThread(int connfd, const InetAddress &remote_addr, const misc::SimpleTimeSptr &happen_st_sptr, const std::string &trace_id) {
  auto tcp_conn_sptr = pool_uptr_->Get();
  if (tcp_conn_sptr == nullptr) {
    tcp_conn_sptr = std::make_shared<TcpConnection>(connfd, remote_addr, loop_uptr_.get());
  } else {
    tcp_conn_sptr->Reuse(connfd, remote_addr, loop_uptr_.get());
  }

  tcp_conn_sptr->set_trace_id(trace_id);

  if (server_ptr_->connected_callback_ != nullptr) {
    tcp_conn_sptr->set_connected_callback(server_ptr_->connected_callback_);
  }

  tcp_conn_sptr->set_disconnected_callback(
          std::bind(&TcpServer::ConnectionThread::DisconnectedCallback,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2));

  tcp_conn_sptr->set_read_callback(
          std::bind(&TcpServer::ConnectionThread::ConnectionReadCallback,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2));

  if (server_ptr_->write_complete_callback_ != nullptr) {
    tcp_conn_sptr->set_write_complete_callback(server_ptr_->write_complete_callback_);
  }

  if (server_ptr_->error_callback_ != nullptr) {
    tcp_conn_sptr->set_error_callback(server_ptr_->error_callback_);
  }

  auto timeout_seconds = tcp_conn_sptr->timeout_seconds();
  if (timeout_seconds == 0) {
    timeout_seconds = server_ptr_->default_conn_idle_seconds_;
  }

  conn_time_hand_map_.emplace(connfd, time_wheel_uptr_->AddConnection(tcp_conn_sptr, timeout_seconds));

  tcp_conn_sptr->ConnectEstablished(happen_st_sptr);
}


}

}