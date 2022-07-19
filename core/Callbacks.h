//
// Created by song on 7/19/2022.
//

#ifndef SERVER_CALLBACKS_H
#define SERVER_CALLBACKS_H

#include <functional>
#include <memory>

class TcpConnection;

class Buffer;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void()> TimerCallback;
typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr &, Buffer *buffer)> MessageCallback;
typedef std::function<void(const TcpConnectionPtr &)> CloseCallback;
typedef std::function<void(const TcpConnectionPtr &)> WriteCompleteCallback;

#endif //SERVER_CALLBACKS_H
