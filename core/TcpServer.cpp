//
// Created by song on 7/19/2022.
//

#include "TcpServer.h"

TcpServer::TcpServer() {

}

TcpServer::~TcpServer() {

}

void TcpServer::start(int numThreads) {
    LOG_INFO("server started with %d threads", numThreads);
}

void TcpServer::setConnectionEstablishedCallback(const ConnectionCallback &cb) {

}

void TcpServer::setConnectionClosedCallback(const ConnectionCallback &cb) {

}

void TcpServer::setMessageCallback(const MessageCallback &cb) {

}

void TcpServer::setWriteCompleteCallback(const WriteCompleteCallback &cb) {

}

