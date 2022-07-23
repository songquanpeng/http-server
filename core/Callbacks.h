//
// Created by song on 7/19/2022.
//

#ifndef SERVER_CALLBACKS_H
#define SERVER_CALLBACKS_H

#include <functional>
#include <memory>

class HTTPConnection;

class HTTPRequest;

class HTTPResponse;

class Buffer;

typedef std::shared_ptr<HTTPConnection> HTTPConnectionPtr;
typedef std::shared_ptr<HTTPRequest> HTTPRequestPtr;
typedef std::shared_ptr<HTTPResponse> HTTPResponsePtr;
typedef std::function<void(const std::string &)> HTTPSender;

typedef std::function<void()> TimerCallback;
typedef std::function<void(const HTTPConnectionPtr &)> ConnectionCallback;
typedef std::function<void(const HTTPConnectionPtr &, Buffer *buffer)> MessageCallback;
typedef std::function<void(const HTTPConnectionPtr &)> CloseCallback;
typedef std::function<void(const HTTPConnectionPtr &)> WriteCompleteCallback;

typedef std::function<void(const HTTPRequestPtr &, const HTTPResponsePtr &)> RouteCallback;

#endif //SERVER_CALLBACKS_H
