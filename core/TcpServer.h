//
// Created by song on 7/19/2022.
//

#ifndef SERVER_TCPSERVER_H
#define SERVER_TCPSERVER_H

#include "logger/Logger.h"
#include "Callbacks.h"

class TcpServer {
public:
    TcpServer();

    ~TcpServer();

    void start(int numThreads);

    void setConnectionEstablishedCallback(const ConnectionCallback &cb);

    void setConnectionClosedCallback(const ConnectionCallback &cb);

    void setMessageCallback(const MessageCallback &cb);

    void setWriteCompleteCallback(const WriteCompleteCallback &cb);
};


#endif //SERVER_TCPSERVER_H
