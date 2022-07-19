//
// Created by song on 7/19/2022.
//

#include "TcpConnection.h"

TcpConnection::TcpConnection() {

}

TcpConnection::~TcpConnection() {

}

void TcpConnection::setConnectionEstablishedCallback(const ConnectionCallback &cb) {

}

void TcpConnection::setConnectionClosedCallback(const ConnectionCallback &cb) {

}

void TcpConnection::setMessageCallback(const MessageCallback &cb) {

}

void TcpConnection::setWriteCompleteCallback(const WriteCompleteCallback &cb) {

}

void TcpConnection::setCloseCallback(const CloseCallback &cb) {

}

void TcpConnection::send(const std::string &message) {

}

void TcpConnection::close() {

}

void TcpConnection::connectionEstablished() {

}

void TcpConnection::connectionDestroyed() {

}

void TcpConnection::sendInLoop(const std::string &message) {

}

void TcpConnection::closeInLoop() {

}

