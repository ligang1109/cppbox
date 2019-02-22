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

TcpServer::TcpServer(uint16_t port, const log::LoggerSptr &logger_sptr, const std::string &ip) :
        ip_(ip),
        port_(port),
        logger_sptr_(logger_sptr),
        loop_uptr_(nullptr),
        dispatch_index_(0) {
  if (logger_sptr_ == nullptr) {
    logger_sptr_.reset(new log::NullLogger());
  }
}

void TcpServer::set_new_conn_func(const NewConnectionFunc &func) {
  new_conn_func_ = func;
}

void TcpServer::set_connected_callback(const TcpConnCallback &cb) {
  connected_callback_ = cb;
}

void TcpServer::set_disconnected_callback(const TcpConnCallback &cb) {
  disconnected_callback_ = cb;
}

void TcpServer::set_read_callback(const TcpConnCallback &cb) {
  read_callback_ = cb;
}

void TcpServer::set_write_complete_callback(const TcpConnCallback &cb) {
  write_complete_callback_ = cb;
}

void TcpServer::set_error_callback(const TcpConnCallback &cb) {
  error_callback_ = cb;
}

misc::ErrorUptr TcpServer::Init(int conn_thread_cnt, int conn_thread_loop_timeout_ms) {
  listenfd_ = NewTcpIpV4NonBlockSocket();
  if (listenfd_ == -1) {
    return misc::NewErrorUptrByErrno();
  }

  loop_uptr_.reset(new EventLoop(logger_sptr_));
  auto err_uptr = loop_uptr_->Init();
  if (err_uptr != nullptr) {
    return err_uptr;
  }

  for (auto i = 0; i < conn_thread_cnt; ++i) {
    auto conn_thread_uptr = misc::MakeUnique<ConnectionThread>(i, this);
    err_uptr = conn_thread_uptr->Init(conn_thread_loop_timeout_ms);
    if (err_uptr != nullptr) {
      return err_uptr;
    }

    conn_thread_list_.push_back(std::move(conn_thread_uptr));
  }

  return nullptr;
}


misc::ErrorUptr TcpServer::Start() {
  if (new_conn_func_ == nullptr) {
    new_conn_func_ = std::bind(
            &TcpServer::DefaultNewConnection,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3);
  }

  for (auto &conn_thread_uptr : conn_thread_list_) {
    conn_thread_uptr->Start();
  }

  return ListenAndServe();
}

size_t TcpServer::ConnectionCount() {
  size_t cnt = 0;
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

void TcpServer::RunAtTimeInConnectionThread(time_t abs_sec, const Event::EventCallback &cb) {
  for (auto &conn_thread_uptr : conn_thread_list_) {
    conn_thread_uptr->RunAtTime(abs_sec, cb);
  }
}

void TcpServer::RunAfterTimeInConnectionThread(time_t delay_sec, const Event::EventCallback &cb) {
  for (auto &conn_thread_uptr : conn_thread_list_) {
    conn_thread_uptr->RunAfterTime(delay_sec, cb);
  }
}

void TcpServer::RunEveryTimeInConnectionThread(time_t interval_sec, const Event::EventCallback &cb) {
  for (auto &conn_thread_uptr : conn_thread_list_) {
    conn_thread_uptr->RunEveryTime(interval_sec, cb);
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

void TcpServer::ListenCallback(const misc::SimpleTimeSptr &happened_st_sptr) {
  InetAddress raddr;

  int connfd;
  while (true) {
    connfd = Accept(listenfd_, raddr);
    if (connfd == -1) {
      if (errno == EINTR) {
        continue;
      }

      logger_sptr_->Error("accept error: " + cppbox::misc::NewErrorUptrByErrno()->String());
      return;
    }

    break;
  }

  conn_thread_list_[dispatch_index_]->AddConnection(connfd, raddr, happened_st_sptr);
  dispatch_index_ = (dispatch_index_ + 1) % conn_thread_list_.size();
}

TcpConnectionSptr TcpServer::DefaultNewConnection(int connfd, const cppbox::net::InetAddress &remote_addr, EventLoop *loop_ptr) {
  return std::make_shared<TcpConnection>(connfd, remote_addr, loop_ptr);
}


TcpServer::ConnectionThread::ConnectionThread(int id, TcpServer *server_ptr) :
        id_(id),
        server_ptr_(server_ptr),
        loop_uptr_(nullptr),
        time_event_sptr_(nullptr),
        has_added_time_event_(false),
        thread_uptr_(nullptr) {}

misc::ErrorUptr TcpServer::ConnectionThread::Init(int loop_timeout_ms) {
  loop_uptr_.reset(new EventLoop(server_ptr_->logger_sptr_, loop_timeout_ms));
  auto err_uptr = loop_uptr_->Init();
  if (err_uptr != nullptr) {
    return err_uptr;
  }

  time_event_sptr_.reset(new TimeEvent());
  err_uptr = time_event_sptr_->Init();
  if (err_uptr != nullptr) {
    return err_uptr;
  }

  return nullptr;
}

void TcpServer::ConnectionThread::Start() {
  thread_uptr_.reset(new std::thread(
          std::bind(&TcpServer::ConnectionThread::ThreadFunc, this)
                                    ));
}

void TcpServer::ConnectionThread::AddConnection(int connfd, const InetAddress &remote_addr, const misc::SimpleTimeSptr &happened_st_sptr) {
  loop_uptr_->AppendFunction(
          std::bind(&TcpServer::ConnectionThread::AddConnectionInThread, this, connfd, remote_addr, happened_st_sptr)
                            );
}

size_t TcpServer::ConnectionThread::ConnectionCount() {
  return conn_map_.size();
}

void TcpServer::ConnectionThread::RunFunction(const EventLoop::Functor &func) {
  loop_uptr_->AppendFunction(func);
}

void TcpServer::ConnectionThread::RunAtTime(time_t abs_sec, const Event::EventCallback &cb) {
  time_event_sptr_->RunAt(abs_sec, cb);
  EnsureAddTimeEvent();
}

void TcpServer::ConnectionThread::RunAfterTime(time_t delay_sec, const Event::EventCallback &cb) {
  time_event_sptr_->RunAfter(delay_sec, cb);
  EnsureAddTimeEvent();
}

void TcpServer::ConnectionThread::RunEveryTime(time_t interval_sec, const Event::EventCallback &cb) {
  time_event_sptr_->RunEvery(interval_sec, cb);
  EnsureAddTimeEvent();
}

void TcpServer::ConnectionThread::ThreadFunc() {
  tcp_conn_thread_id = id_;

  loop_uptr_->Loop();
}

void TcpServer::ConnectionThread::AddConnectionInThread(int connfd, const InetAddress &remote_addr, const misc::SimpleTimeSptr &happened_st_sptr) {
  auto tcp_conn_sptr = server_ptr_->new_conn_func_(connfd, remote_addr, loop_uptr_.get());

  if (server_ptr_->connected_callback_ != nullptr) {
    tcp_conn_sptr->set_connected_callback(server_ptr_->connected_callback_);
  }

  tcp_conn_sptr->set_disconnected_callback(
          std::bind(&TcpServer::ConnectionThread::DisconnectedCallback,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2));

  if (server_ptr_->read_callback_ != nullptr) {
    tcp_conn_sptr->set_read_callback(server_ptr_->read_callback_);
  }
  if (server_ptr_->write_complete_callback_ != nullptr) {
    tcp_conn_sptr->set_write_complete_callback(server_ptr_->write_complete_callback_);
  }
  if (server_ptr_->error_callback_ != nullptr) {
    tcp_conn_sptr->set_error_callback(server_ptr_->error_callback_);
  }

  conn_map_[tcp_conn_sptr->connfd()] = tcp_conn_sptr;
  tcp_conn_sptr->ConnectEstablished();
}

void TcpServer::ConnectionThread::DisconnectedCallback(const TcpConnectionSptr &tcp_conn_sptr, const misc::SimpleTimeSptr &happened_st_sptr) {
  conn_map_.erase(tcp_conn_sptr->connfd());

  if (server_ptr_->disconnected_callback_ != nullptr) {
    server_ptr_->disconnected_callback_(tcp_conn_sptr, happened_st_sptr);
  }
}

void TcpServer::ConnectionThread::EnsureAddTimeEvent() {
  if (!has_added_time_event_) {
    loop_uptr_->UpdateEvent(time_event_sptr_);
    has_added_time_event_ = true;
  }
}

}

}