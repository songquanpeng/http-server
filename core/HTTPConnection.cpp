//
// Created by song on 7/19/2022.
//

#include "HTTPConnection.h"
#include "HTTPServer.h"
#include "Channel.h"
#include <utility>
#include <unistd.h>

using namespace std;

HTTPConnection::HTTPConnection(EventLoop *loop, HTTPServer *server, int sock_fd, std::string name) :
        ownerLoop(loop), ownerServer(server), sock_fd_(sock_fd), name_(std::move(name)),
        channel(new Channel(loop, sock_fd)), request(new HTTPRequest) {
    LOG_INFO("HTTPConnection %d with sock_fd %d initialized", this, sock_fd_);
    channel->setReadCallback(std::bind(&HTTPConnection::handleRead, this));
    channel->setWriteCallback(std::bind(&HTTPConnection::handleWrite, this));
    channel->setErrorCallback(std::bind(&HTTPConnection::handleError, this));
    channel->setCloseCallback(std::bind(&HTTPConnection::handleClose, this));
}

HTTPConnection::~HTTPConnection() {
    LOG_INFO("HTTPConnection %d with sock_fd %d destructed", this, sock_fd_);
}

void HTTPConnection::send(const std::string &message) {
    if (ownerLoop->isInLoopThread()) {
        sendInLoop(message);
    } else {
        ownerLoop->runInLoop(bind(&HTTPConnection::sendInLoop, this, message));
    }
}

void HTTPConnection::close() {

}

void HTTPConnection::connectionEstablished() {
    channel->enableListeningReadEvent();
}

void HTTPConnection::connectionDestroyed() {
    LOG_INFO("HTTPConnection %d with sock_fd %d destroyed", this, sock_fd_);
}

void HTTPConnection::closeInLoop() {

}

void HTTPConnection::setCloseCallback(const CloseCallback &cb) {
    closeCallback_ = cb;
}

std::string HTTPConnection::getName() const {
    return name_;
}

EventLoop *HTTPConnection::getLoop() const {
    return ownerLoop;
}

void HTTPConnection::handleRead() {
    ownerLoop->assertInLoopThread();
    // TODO: improve handleRead like muduo
    char buffer[65536];
    int n = read(sock_fd_, buffer, sizeof buffer);
    if (n < 0) {
        LOG_ERROR("HTTPConnection::handleRead read() return %d", n);
        return;
    }
//    LOG_INFO("HTTPConnection::handleRead get request: %s", buffer);
    // TODO: consider multiple send
    bool ok = parseRequestHead(buffer);
    if (!ok) {
        LOG_ERROR("HTTPConnection::handleRead failed to parse HTTP request: %s", buffer);
    }
    // TODO: parse body
    parseRequestBody("");
    auto cb = ownerServer->getRouteCallback(request->url);
    cb(request, shared_from_this());
}

void HTTPConnection::handleWrite() {
    ownerLoop->assertInLoopThread();
    // TODO: improve handleWrite like muduo
}

void HTTPConnection::sendInLoop(const std::string &message) {
    // TODO: improve sendInLoop like muduo
    string res = buildHTTPResponse(message);
    int n = write(sock_fd_, res.data(), res.size());
}

void HTTPConnection::handleError() {
    LOG_ERROR("HTTPConnection %d with sock_fd %d error happened: %s", this, sock_fd_, errno);
}

void HTTPConnection::handleClose() {
    channel->disableListeningAllEvent();
    // TODO: why shared_from_this
    closeCallback_(shared_from_this());
}

bool HTTPConnection::parseRequestHead(const std::string &data) {
    vector<string> items;
    string s;
    for (auto c: data) {
        if (c == '\r') {
            items.push_back(s);
            s = "";
        } else if (c != '\n') {
            s += c;
        }
    }
    if (!items.empty()) {
        int i = 0;
        string tmp;
        for (auto c: items[0]) {
            if (c == ' ') {
                if (i == 0) {
                    request->method = tmp;
                } else {
                    request->url = tmp;
                }
                tmp = "";
                i++;
            } else {
                tmp += c;
            }
        }
    } else {
        return false;
    }
    for (int i = 1; i < items.size(); i++) {
        string key;
        string tmp;
        for (int j = 0; j < items[i].size(); j++) {
            if (items[i][j] == ':' && !key.empty()) {
                key = tmp;
                tmp = "";
                j++; // skip next space
            } else {
                tmp += items[i][j];
            }
        }
        request->headers[key] = tmp;
    }
    return true;
}

bool HTTPConnection::parseRequestBody(const std::string &data) {
    return false;
}

string HTTPConnection::buildHTTPResponse(const string &data) {
    string res = "HTTP/1.1 200 OK\n"
                 "Content-Type: text/html\n"
                 "Content-Length: " + to_string(data.size()) + "\n"
                 "\n" + data;
    return res;
}


