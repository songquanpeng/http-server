//
// Created by song on 7/19/2022.
//

#ifndef SERVER_HTTPSERVER_H
#define SERVER_HTTPSERVER_H

#include "Logger.h"
#include "Callbacks.h"
#include "Channel.h"
#include <unordered_map>
#include <string>

class EventLoop;

class HTTPServer {
public:
    HTTPServer(EventLoop *loop, int port, int numThreads);

    ~HTTPServer();

    void setRoute(const std::string &prefix, const RouteCallback &cb);

    RouteCallback getRouteCallback(const std::string &url);

    void start();

private:
    int setUpSocket(int port);

    EventLoop *loop_;
    int socket_fd_;
    std::unordered_map<std::string, RouteCallback> routeMap;
    Channel acceptChannel;
    bool started = false;
    typedef std::map<std::string, HTTPConnectionPtr> ConnectionMap;
    ConnectionMap connectionMap;

    void newConnection();
    void removeConnection(const HTTPConnectionPtr& conn);
    void removeConnectionInLoop(const HTTPConnectionPtr& conn);

    void noRoute(const HTTPRequestPtr &req, const HTTPConnectionPtr &res);
};


#endif //SERVER_HTTPSERVER_H
