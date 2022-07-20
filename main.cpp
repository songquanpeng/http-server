#include "HTTPServer.h"
#include "EventLoop.h"
#include "HTTPConnection.h"

using namespace std;

void getTestPage(const HTTPRequestPtr &req, const HTTPConnectionPtr &res) {
    LOG_INFO("getTestPage");
    string html = "<h1>Hello World!</h2>";
    res->send(html);
}

int main() {
    EventLoop loop;
    HTTPServer server(&loop, 3000, 4);
    server.setRoute("/test", getTestPage);
    server.start();
    loop.loop();
    return 0;
}
