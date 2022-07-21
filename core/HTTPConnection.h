//
// Created by song on 7/19/2022.
//

#ifndef SERVER_HTTPCONNECTION_H
#define SERVER_HTTPCONNECTION_H

#include <string>
#include <memory>

#include "Callbacks.h"
#include "HTTPRequest.h"


// TODO: why this?
class EventLoop;

class HTTPServer;

class Channel;

class HTTPConnection : public std::enable_shared_from_this<HTTPConnection> {
public:
    HTTPConnection(EventLoop *loop, HTTPServer *server, int sock_fd, std::string name);

    ~HTTPConnection();

    /// Called by user to send data
    void send(const std::string &message);

    /// Called by user to end this connection
    void close();

    /// Called by HTTPServer
    void connectionEstablished();

    /// Called by HTTPServer
    void connectionDestroyed();

    void setCloseCallback(const CloseCallback &cb);

    std::string getName() const;

    EventLoop *getLoop() const;

private:
    void sendInLoop(const std::string &message);

    void closeInLoop();

    void handleRead();

    void handleWrite();

    void handleError();

    void handleClose();

    bool parseRequestHead(const std::string &data);

    bool parseRequestBody(const std::string &data);

    std::string buildHTTPResponse(const std::string &data);

    HTTPServer *ownerServer;
    EventLoop *ownerLoop;
    std::shared_ptr<HTTPRequest> request;
    std::unique_ptr<Channel> channel;
    int sock_fd_;
    CloseCallback closeCallback_;
    std::string name_;
};


#endif //SERVER_HTTPCONNECTION_H
