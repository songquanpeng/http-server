#include "HTTPServer.h"
#include "EventLoop.h"
#include "HTTPConnection.h"

using namespace std;

void getIndexPage(const HTTPRequestPtr &req, const HTTPConnectionPtr &res) {
    LOG_INFO("getIndexPage");
    string html = "<h1>Hello World!</h1>";
    res->send(html);
}

int main() {
    EventLoop loop;
    HTTPServer server(&loop, 3000, 4);
    server.setRoute("/", getIndexPage);
    server.start();
    loop.loop();
    return 0;
}
