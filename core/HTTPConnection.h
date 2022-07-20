//
// Created by song on 7/19/2022.
//

#ifndef SERVER_HTTPCONNECTION_H
#define SERVER_HTTPCONNECTION_H

#include <string>

#include "Callbacks.h"

class HTTPConnection {
public:
    HTTPConnection();

    ~HTTPConnection();

    /// Called by user to send data
    void send(const std::string &message);

    /// Called by user to end this connection
    void close();

    /// Called by HTTPServer
    void connectionEstablished();

    /// Called by HTTPServer
    void connectionDestroyed();

private:
    void sendInLoop(const std::string &message);

    void closeInLoop();
};


#endif //SERVER_HTTPCONNECTION_H
