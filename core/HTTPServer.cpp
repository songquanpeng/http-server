//
// Created by song on 7/19/2022.
//

#include "HTTPServer.h"
#include <arpa/inet.h>
#include <endian.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include "EventLoop.h"
#include <unistd.h>
#include "Channel.h"
#include <functional>
#include "HTTPConnection.h"


HTTPServer::HTTPServer(EventLoop *loop, int port, int numThreads) :
        loop_(loop), socket_fd_(setUpSocket(port)), acceptChannel(loop, socket_fd_) {
    // TODO: use EventLoop po
    acceptChannel.setReadCallback(std::bind(&HTTPServer::newConnection, this));
}


HTTPServer::~HTTPServer() {
    LOG_INFO("Server stopped");
}

void HTTPServer::start() {
    if (!started) {
        LOG_INFO("Server started");
        loop_->assertInLoopThread();
        // TODO: SOMAXCONN
        int ret = listen(socket_fd_, SOMAXCONN);
        if (ret < 0) {
            LOG_FATAL("Failed to listen on socket");
        }
        acceptChannel.enableListeningReadEvent();
        started = true;
    }
}

void HTTPServer::setRoute(const std::string &prefix, const RouteCallback &cb) {
    if (started) {
        LOG_WARN("Trying to register route handler but server already started");
    } else {
        routeMap[prefix] = cb;
    }
}

int HTTPServer::setUpSocket(int port) {
    int socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (socket_fd < 0) {
        LOG_FATAL("Failed to get socket fd");
    }
    // TODO: SO_REUSEADDR
    // https://www.cnblogs.com/mydomain/archive/2011/08/23/2150567.html
    int opt_val = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);
    // bind socket
    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    // TODO: reinterpret_cast vs static_cast
    int ret = bind(socket_fd, reinterpret_cast<const sockaddr *>(&address), sizeof address);
    if (ret < 0) {
        LOG_FATAL("Failed to bind socket to port %d", port);
    }
    LOG_INFO("Server socket bind to %d", port);
    return socket_fd;
}

void HTTPServer::newConnection() {
    loop_->assertInLoopThread();
    struct sockaddr_in addr{};
    bzero(&addr, sizeof addr);
    socklen_t addrlen = sizeof addr;
    // TODO: accept4 vs accept
    int conn_fd = accept4(socket_fd_, reinterpret_cast<sockaddr *>(&addr), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (conn_fd < 0) {
        LOG_ERROR("HTTPServer::newConnection accept failed with errno %d", errno);
        return;
    }
    char buf[32];
    char host[INET_ADDRSTRLEN] = "INVALID";
    ::inet_ntop(AF_INET, &addr.sin_addr, host, sizeof host);
    uint16_t port = ntohs(addr.sin_port);
    snprintf(buf, sizeof buf, "%s:%u", host, port);
    LOG_INFO("New connection with conn fd %d from %s", conn_fd, buf);

    // TODO: get loop from loop poll
    std::string connName = buf;
    HTTPConnectionPtr conn(new HTTPConnection(loop_, this, conn_fd, connName));
    connectionMap[connName] = conn;
    conn->setCloseCallback(std::bind(&HTTPServer::removeConnection, this, conn));
    loop_->runInLoop(std::bind(&HTTPConnection::connectionEstablished, conn));
}

RouteCallback HTTPServer::getRouteCallback(const std::string &url) {
    // TODO: url -> route mapping
    // TODO: 404 route
    if (routeMap.count(url)) {
        return routeMap[url];
    }
    return [](const HTTPRequestPtr &req, const HTTPConnectionPtr &res) {
        res->send("<html>\n"
                  "<head><title>404 Not Found</title></head>\n"
                  "<body bgcolor=\"white\">\n"
                  "<center><h1>404 Not Found</h1></center>\n"
                  "<hr><center>JustSong's Server</center>\n"
                  "</body>\n"
                  "</html>");
    };
}

void HTTPServer::removeConnection(const HTTPConnectionPtr &conn) {
    if (loop_->isInLoopThread()) {
        removeConnectionInLoop(conn);
    } else {
        loop_->runInLoop(std::bind(&HTTPServer::removeConnectionInLoop, this, conn));
    }
}

void HTTPServer::removeConnectionInLoop(const HTTPConnectionPtr &conn) {
    routeMap.erase(conn->getName());
    auto loop = conn->getLoop();
    // TODO: why not HTTPConnection destroy by itself?
    loop->runInLoop(std::bind(&HTTPConnection::connectionDestroyed, conn));
}



