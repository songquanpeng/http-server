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
        ownerLoop(loop),
        ownerServer(server),
        sock_fd_(sock_fd),
        name_(std::move(name)),
        channel(new Channel(loop, sock_fd)) {
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
    channel->disableListeningAllEvent();
    ownerLoop->removeChannel(channel.get());
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
    auto n = inputBuffer.readFd(sock_fd_);
    if (n < 0) {
        LOG_ERROR("HTTPConnection::handleRead inputBuffer.readFd return %d", n);
        return;
    } else if (n == 0) {
        // This means the connection can be close now.
        handleClose();
        return;
    }
    if (needNMoreBytes == -1 || inputBuffer.readableBytes() >= needNMoreBytes) {
        ownerLoop->runInLoop(std::bind(&HTTPConnection::tryBuildRequestAndProcess, this));
    }
}

void HTTPConnection::tryBuildRequestAndProcess() {
    bool ok = false;

    // Maybe not enough.
    std::string data(inputBuffer.peek(), inputBuffer.readableBytes());
    int onlyNeedNBytes = 0;
    shared_ptr<HTTPRequest> request(new HTTPRequest);
    auto ret = parseRequest(data, onlyNeedNBytes, request);
    if (ret == 0) {
        ok = true;
        inputBuffer.retrieve(onlyNeedNBytes);
        needNMoreBytes = -1;
    } else if (ret == -1) {
        // Maybe not enough, we will try again.
        ok = false;
    } else {
        needNMoreBytes = ret;
    }

    if (ok) {
        auto cb = ownerServer->getRouteCallback(request->url);
//        cb(request, shared_from_this());
        shared_ptr<HTTPResponse> response(new HTTPResponse);
        response->setSender(bind(&HTTPConnection::send, shared_from_this(), std::placeholders::_1));
        ownerLoop->runInLoop(bind(cb, request, response));
    }
}

/// Results: return 0 means okay, return -1 means error, return positive number n means need n more bytes.
int HTTPConnection::parseRequest(const std::string &data, int &onlyNeedNBytes, const shared_ptr<HTTPRequest> &request) {
    // Find split index
    auto splitIdx = data.find("\r\n\r\n");
    if (splitIdx == string::npos) {
        return -1;
    }
    // Construct header
    vector<string> items;
    string s;
    for (int i = 0; i < splitIdx; i++) {
        char c = data[i];
        if (c == '\r') {
            items.push_back(s);
            s = "";
        } else if (c != '\n') {
            s += c;
        }
    }
    if (items.empty()) {
        return -1;
    }
    // Parse the first line
    {
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
    }
    // Parse the rest lines of header
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
    if (request->headers.count("Content-Length") == 0) {
        // Body is empty
        request->bodySize == 0;
        onlyNeedNBytes = splitIdx + 4;
        return 0;
    }
    // Body is not empty
    request->bodySize = stoi(request->headers["Content-Length"]);
    if (request->bodySize + splitIdx + 4 > data.size()) {  // Add 4 because \r\n\r\n
        // Not enough, body is incomplete
        return request->bodySize + splitIdx + 4;
    }
    onlyNeedNBytes = request->bodySize + splitIdx + 4;
    return 0;
}

void HTTPConnection::sendInLoop(const std::string &response) {
    ownerLoop->assertInLoopThread();
    ssize_t numWroteBytes = 0;
    if (!channel->isWriting() && outputBuffer.readableBytes() == 0) {
        // Channel clear, we can write now.
        numWroteBytes = write(sock_fd_, response.data(), response.size());
        if (numWroteBytes >= 0) {
            if (numWroteBytes == response.size()) {
                // Write completed.
            }
        } else {
            // TODO: EWOULDBLOCK
            if (errno != EWOULDBLOCK) {
                LOG_ERROR("HTTPConnection::sendInLoop write return %d with errno %d", numWroteBytes, errno);
            }
        }
    }
    // Save the unwritten bytes to output buffer.
    if (numWroteBytes < response.size()) {
        outputBuffer.append(response.data() + numWroteBytes, response.size() - numWroteBytes);
        if (!channel->isWriting()) {
            channel->enableListeningWriteEvent();
        }
    }
}

void HTTPConnection::handleWrite() {
    ownerLoop->assertInLoopThread();
    // TODO: improve handleWrite like muduo
    auto n = write(sock_fd_, outputBuffer.peek(), outputBuffer.readableBytes());
    if (n < 0) {
        if (errno != EWOULDBLOCK) {
            LOG_ERROR("HTTPConnection::sendInLoop write return %d with errno %d", n, errno);
        }
    } else {
        if (n == outputBuffer.readableBytes()) {
            // Write completed.
            channel->disableListeningWriteEvent();
        }
        outputBuffer.retrieve(n);
    }
}

void HTTPConnection::handleError() {
    LOG_ERROR("HTTPConnection %d with sock_fd %d error happened: %s", this, sock_fd_, errno);
}

void HTTPConnection::handleClose() {
    channel->disableListeningAllEvent();
    // TODO: why shared_from_this
    closeCallback_(shared_from_this());
}

