#include "HTTPServer.h"
#include "EventLoop.h"
#include "HTTPConnection.h"

using namespace std;

void getIndexPage(const HTTPRequestPtr &req, const HTTPResponsePtr &res) {
    LOG_INFO("getIndexPage");
    res->send("<h1>Hello World!</h1><a href='/echo'>Go To Echo Page</a>");
}

void getEchoPage(const HTTPRequestPtr &req, const HTTPResponsePtr &res) {
    LOG_INFO("getEchoPage");
    res->send("<li> Method: " + req->method + "</li><li> URL: " + req->url + "</li>");
}

int main() {
    EventLoop loop;
    HTTPServer server(&loop, 3000, 4);
    server.setRoute("/", getIndexPage);
    server.setRoute("/echo", getEchoPage);
    server.start();
    loop.loop();
    return 0;
}
