//
// Created by song on 7/19/2022.
//

#ifndef SERVER_HTTPCONNECTION_H
#define SERVER_HTTPCONNECTION_H

#include <string>
#include <memory>

#include "Callbacks.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "Buffer.h"


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
    enum State {
        kConnecting,
        kConnected,
        kDisconnecting,
        kDisconnected
    };

    void sendInLoop(const std::string &response);

    void closeInLoop();

    void handleRead();

    void handleWrite();

    void handleError();

    void handleClose();

    void tryBuildRequestAndProcess();

    static int parseRequest(const std::string &data, int &onlyNeedNBytes, const std::shared_ptr<HTTPRequest> &request);

    HTTPServer *ownerServer;
    EventLoop *ownerLoop;
    std::unique_ptr<Channel> channel;
    int sock_fd_;
    CloseCallback closeCallback_;
    std::string name_;
    // This is used by tryBuildRequestAndProcess() & handleRead()
    int needNMoreBytes = -1;

    Buffer inputBuffer;
    Buffer outputBuffer;
};


#endif //SERVER_HTTPCONNECTION_H
