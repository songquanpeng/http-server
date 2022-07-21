#include "HTTPServer.h"
#include "EventLoop.h"
#include "HTTPConnection.h"

using namespace std;

void getIndexPage(const HTTPRequestPtr &req, const HTTPConnectionPtr &res) {
    LOG_INFO("getIndexPage");
    string html = "<h1>Hello World!</h1><a href='/echo'>Go To Echo Page</a>";
    res->send(html);
}

void getEchoPage(const HTTPRequestPtr &req, const HTTPConnectionPtr &res) {
    LOG_INFO("getEchoPage");
    string html = "<li> Method: " + req->method + "</li><li> URL: " + req->url + "</li>";
    res->send(html);
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
