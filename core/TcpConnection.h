//
// Created by song on 7/19/2022.
//

#ifndef SERVER_TCPCONNECTION_H
#define SERVER_TCPCONNECTION_H

#include <string>

#include "Callbacks.h"

class TcpConnection {
public:
    TcpConnection();

    ~TcpConnection();

    /// Called by TcpServer to register user's callback
    void setConnectionEstablishedCallback(const ConnectionCallback &cb);

    /// Called by TcpServer to register user's callback
    void setConnectionClosedCallback(const ConnectionCallback &cb);

    /// Called by TcpServer to register user's callback
    void setMessageCallback(const MessageCallback &cb);

    /// Called by TcpServer to register user's callback
    void setWriteCompleteCallback(const WriteCompleteCallback &cb);

    /// Called by TcpServer to register user's callback
    void setCloseCallback(const CloseCallback &cb);

    /// Called by user to send data
    void send(const std::string &message);

    /// Called by user to end this connection
    void close();

    /// Called by TcpServer
    void connectionEstablished();

    /// Called by TcpServer
    void connectionDestroyed();

private:
    void sendInLoop(const std::string &message);

    void closeInLoop();
};


#endif //SERVER_TCPCONNECTION_H
