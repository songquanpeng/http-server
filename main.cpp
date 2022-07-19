#include "TcpServer.h"
#include "EventLoop.h"

using namespace std;

void onConnectionEstablished(const TcpConnectionPtr &conn) {
    LOG_INFO("onConnectionEstablished");
}

void onConnectionClosed(const TcpConnectionPtr &conn) {
    LOG_INFO("onConnectionClosed");
}

void onMessage(const TcpConnectionPtr &conn, Buffer *buffer) {
    LOG_INFO("onMessage");
}

void onWriteComplete(const TcpConnectionPtr &conn) {
    LOG_INFO("onWriteComplete");
}

int main() {
    TcpServer server;
    EventLoop loop;
    server.setConnectionEstablishedCallback(onConnectionEstablished);
    server.setMessageCallback(onMessage);
    server.setWriteCompleteCallback(onWriteComplete);
    server.setConnectionClosedCallback(onConnectionClosed);
    server.start(4);
    loop.loop();
    return 0;
}
